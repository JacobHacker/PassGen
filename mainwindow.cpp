#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <QDebug>
#include <QTime>
#include <QUrl>

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
	// Set GUI to variables
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

	m_downloadingBar = new QProgressBar;

	ui_algorithmCombo->addItem("Random.org");
	ui_algorithmCombo->addItem("Mersenne Twister");
	ui_algorithmCombo->addItem("rand()");

	connect(ui_customCheck, SIGNAL(stateChanged(int)),
			this, SLOT( customCharsetCheckPressed() ));

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

			ui_randomLine->setText( password );
		}
	}
}

void MainWindow::on_generateBtn_clicked()
{
	uint passLen = ui_lengthSpin->value();

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
	if( ui_customCharSetLine->isEnabled() ){
		m_charset += ui_customCharSetLine->text();
	}

	// If no check boxes are down, quit
	if( m_charset.isEmpty() ){
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
		ui_randomLine->setText( password );
	}
	//////////////////
	// 1/2 Random.org
	//////////////////
	else if(ui_algorithmCombo->currentText() == "Random.org"){
		QUrl url("https://www.random.org");
		url.setPath("/integers/");
		url.addQueryItem( "num", QString::number(passLen) );
		url.addQueryItem( "min", "1" );
		url.addQueryItem( "max", QString::number( m_charset.size() ) );
		url.addQueryItem( "col", "1" );
		url.addQueryItem( "base", "10" );
		url.addQueryItem( "format", "plain" );
		url.addQueryItem( "rnd", "new" );

		// Prepare request
		QNetworkRequest req( url );

		ui_statusBar->showMessage( "Downloading random numbers ..." );
		m_netReply = m_netManager->get( req );
		connect( m_netReply, SIGNAL( finished() ),
				this, SLOT( handleRandomOrgReply() ) );
	}
}
