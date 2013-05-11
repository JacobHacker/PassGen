#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QMessageBox>
#include <QSignalMapper>

namespace CharSet{
	QString upperCase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	QString lowerCase = "abcdefghijklmnopqrstuvwxyz";
	QString numbers = "0123456789";
	// Note, this is not the entire puctuation list, because login boxes SHOULD
	// filter out some of the characters to prevent SQL injection.
	// TODO: Make sure this is an accurate list
	// http://tekkna.org/article/sql-injection
	QString punct = ",.!_@$^&*()_";
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	////////////////////////
	// Grab widgets from ui form
	////////////////////////
	ui_mainGridLayout = qFindChild<QGridLayout*>(this, "mainGridLayout");
	ui_randomLine = qFindChild<QLineEdit*>(this, "randomLine");
	ui_lengthSpin = qFindChild<QSpinBox*>(this, "lengthSpin");
	ui_algorithmCombo = qFindChild<QComboBox*>(this, "algorithmCombo");
	ui_lettersCheck = qFindChild<QCheckBox*>(this, "lettersCheck");
	ui_numbersCheck = qFindChild<QCheckBox*>(this, "numbersCheck");
	ui_punctCheck = qFindChild<QCheckBox*>(this, "punctCheck");
	ui_customCheck = qFindChild<QCheckBox*>(this, "customCheck");
	ui_customCharSetLine = qFindChild<QLineEdit*>(this, "customCharSetLine");
	ui_statusBar = qFindChild<QStatusBar*>(this, "statusBar");

    ui_generateBtn = qFindChild<QPushButton*>(this, "generateBtn");
    ui_appendBtn = qFindChild<QPushButton*>(this, "appendBtn");
    ui_spinBtn = qFindChild<QPushButton*>(this, "spinBtn");

	m_downloadingBar = new QProgressBar;

	// Add options to combo box
	ui_algorithmCombo->addItem("Random.org");
	ui_algorithmCombo->addItem("Mersenne Twister");
	ui_algorithmCombo->addItem("rand()");

    // Custom charset checkbox
	connect(ui_customCheck, SIGNAL(stateChanged(int)),
			this, SLOT( customCharsetCheckPressed() ));
    // Append Button
    connect(ui_appendBtn, SIGNAL(clicked()),
            this, SLOT(appendBtnClicked()));
    // Spin Button
    connect(ui->spinBtn, SIGNAL(clicked()),
            this, SLOT(spinData()));

	///////////////////////
	// Random Number seeds
	///////////////////////
	srandom( time(0) );
	m_mTwister.seed( time(0) );

	// Networking stuff
	m_netManager = new QNetworkAccessManager;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::customCharsetCheckPressed(){
	if( ui_customCheck->isChecked() == true ){
		ui_customCharSetLine->setEnabled( true );
	}else{
		ui_customCharSetLine->setEnabled( false );
	}
}

void MainWindow::handleRandomOrgReply(){
	if( m_netReply ){
		uint passLen = ui_lengthSpin->value();

		if( m_netReply->error() == QNetworkReply::NoError ){
			ui_statusBar->showMessage("Finished.", 3);

			QString password("");
			for( uint i=0; i<passLen; ++i ){
				uint num = QString( m_netReply->readLine() ).toUInt();
				password[i] = m_charset[num];
			}
            if(m_append == true) password = password + ui->randomLine->text();
			ui_randomLine->setText( password );
		}
	}
}

void MainWindow::genPass(int append){
    uint passLen = ui_lengthSpin->value();
    Q_ASSERT(passLen != 0);
    if(passLen == 0){
        QMessageBox msgBox;
        msgBox.setText("Password length must be greater than 0.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    ////////////////////
    // Generate Charset
    ////////////////////
    m_charset.clear();
    if( ui_lettersCheck->isChecked() ){
        m_charset += CharSet::lowerCase;
        m_charset += CharSet::upperCase;
    }

    if( ui_numbersCheck->isChecked() ){
        m_charset += CharSet::numbers;
    }

    if( ui_punctCheck->isChecked() ){
        m_charset += CharSet::punct;
    }

    // Custom charset
    if( ui_customCharSetLine->isEnabled() &&
            ! ui_customCharSetLine->text().isEmpty() ){
        m_charset += ui_customCharSetLine->text();
    }

    // If no check boxes are down, quit
    if( m_charset.isEmpty() ){
        QMessageBox msgBox;
        msgBox.setText("You must have at least 1 charset.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    ui_statusBar->clearMessage();

    //////////
    // Rand()
    //////////
    if(ui_algorithmCombo->currentText() == "rand()"){
        QString password("");

        for( uint i=0; i<passLen; ++i ){
            password[i] = m_charset[rand()%m_charset.size()];
        }
        if(append == true) password = password + ui->randomLine->text();
        ui_randomLine->setText( password );
    }
    ////////////
    // Mersenne
    ////////////
    else if( ui_algorithmCombo->currentText() == "Mersenne Twister" ){
        QString password("");

        boost::uniform_int<> range( 0, m_charset.size()-1 );
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
                gen(m_mTwister, range);

        for( uint i=0; i<passLen; ++i ){
            password[i] = m_charset[gen()];
        }
        if(append == true) password = password + ui->randomLine->text();
        ui_randomLine->setText( password );
    }
    //////////////////
    // 1/2 Random.org
    //////////////////
    else if(ui_algorithmCombo->currentText() == "Random.org"){
        QUrl url("https://www.random.org");
        url.setPath("/integers/");
        url.addQueryItem( "num", QString::number(passLen) );
        url.addQueryItem( "min", "0" );
        url.addQueryItem( "max", QString::number( m_charset.size()-1 ) );
        url.addQueryItem( "col", "1" );
        url.addQueryItem( "base", "10" );
        url.addQueryItem( "format", "plain" );
        url.addQueryItem( "rnd", "new" );

        // Prepare request
        QNetworkRequest req( url );

        ui_statusBar->showMessage( "Downloading random numbers ..." );
        m_netReply = m_netManager->get( req );
        // Hacky and annoying
        m_append = append;
        connect( m_netReply, SIGNAL( finished() ),
                this, SLOT( handleRandomOrgReply() ) );
    }
}

void MainWindow::on_generateBtn_clicked(){
    genPass(false);
}

void MainWindow::appendBtnClicked(){
    genPass(true);
}

void MainWindow::spinData(){
    QString text = ui_randomLine->text();

    uint textSize = text.size();
    if(textSize == 0){
        QMessageBox msgBox;
        msgBox.setText("You must have characters to spin.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    // Put the first character in a random place in the string
    for(uint i=0; i<100; i++){
        text.insert(rand()%textSize+1, text[0]);
        text.remove(0,1);
    }
    ui->randomLine->setText(text);
}
