#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QUrlQuery>
#include <QMessageBox>
#include <QSignalMapper>

namespace CharSet{
	QString upperCase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	QString lowerCase = "abcdefghijklmnopqrstuvwxyz";
	QString numbers = "0123456789";
    // Some sites may filter out certain characters
    QString punct = "!@#$%^&*()_+{}|:\"<>?~-=";
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	m_downloadingBar = new QProgressBar;

	// Add options to combo box
    ui->algorithmCombo->addItem("Random.org");
    ui->algorithmCombo->addItem("Mersenne Twister");
    ui->algorithmCombo->addItem("rand()");

    // Custom charset checkbox
    connect(ui->customCheck, SIGNAL(stateChanged(int)),
			this, SLOT( customCharsetCheckPressed() ));
    // Append Button
    connect(ui->appendBtn, SIGNAL(clicked()),
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
    if( ui->customCheck->isChecked() == true ){
        ui->customCharSetLine->setEnabled( true );
	}else{
        ui->customCharSetLine->setEnabled( false );
	}
}

void MainWindow::handleRandomOrgReply(){
	if( m_netReply ){
        uint passLen = ui->lengthSpin->value();

		if( m_netReply->error() == QNetworkReply::NoError ){
            ui->statusBar->showMessage("Finished.", 3);

			QString password("");
			for( uint i=0; i<passLen; ++i ){
				uint num = QString( m_netReply->readLine() ).toUInt();
				password[i] = m_charset[num];
			}
            if(m_append == true) password = password + ui->randomLine->text();
            ui->randomLine->setText( password );
		}
	}
}

void MainWindow::genPass(int append){
    uint passLen = ui->lengthSpin->value();
    Q_ASSERT(passLen > 0);

    ////////////////////
    // Populate Charset
    ////////////////////
    m_charset.clear();

    if( ui->lettersCheck->isChecked() ){
        m_charset += CharSet::lowerCase;
        m_charset += CharSet::upperCase;
    }

    if( ui->numbersCheck->isChecked() ){
        m_charset += CharSet::numbers;
    }

    if( ui->punctCheck->isChecked() ){
        m_charset += CharSet::punct;
    }

    // Custom charset
    if( ui->customCharSetLine->isEnabled() &&
            ! ui->customCharSetLine->text().isEmpty() ){
        m_charset += ui->customCharSetLine->text();
    }

    // If no check boxes are down, quit
    if( m_charset.isEmpty() ){
        QMessageBox msgBox;
        msgBox.setText("You must have at least 1 charset.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    ui->statusBar->clearMessage();

    //////////
    // Rand()
    //////////
    if(ui->algorithmCombo->currentText() == "rand()"){
        QString password("");

        for( uint i=0; i<passLen; ++i ){
            password[i] = m_charset[rand()%m_charset.size()];
        }
        if(append == true) password = password + ui->randomLine->text();
        ui->randomLine->setText( password );
    }
    ////////////
    // Mersenne
    ////////////
    else if( ui->algorithmCombo->currentText() == "Mersenne Twister" ){
        QString password("");

        boost::uniform_int<> range( 0, m_charset.size()-1 );
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
                gen(m_mTwister, range);

        for( uint i=0; i<passLen; ++i ){
            password[i] = m_charset[gen()];
        }
        if(append == true) password = password + ui->randomLine->text();
        ui->randomLine->setText( password );
    }
    //////////////////
    // 1/2 Random.org
    //////////////////
    else if(ui->algorithmCombo->currentText() == "Random.org"){
        QUrl url("https://www.random.org");
        url.setPath("/integers/");
        QUrlQuery q;
        q.addQueryItem( "num", QString::number(passLen) );
        q.addQueryItem( "min", "0" );
        q.addQueryItem( "max", QString::number( m_charset.size()-1 ) );
        q.addQueryItem( "col", "1" );
        q.addQueryItem( "base", "10" );
        q.addQueryItem( "format", "plain" );
        q.addQueryItem( "rnd", "new" );

        url.setQuery(q);

        // Prepare request
        QNetworkRequest req( url );

        ui->statusBar->showMessage( "Downloading random numbers ..." );
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
    QString text = ui->randomLine->text();

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
