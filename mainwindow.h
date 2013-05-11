#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <boost/random/mersenne_twister.hpp>

#include <QMainWindow>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStatusBar>
#include <QProgressBar>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSignalMapper>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_generateBtn_clicked();
    void handleRandomOrgReply();
	void customCharsetCheckPressed();
    void appendBtnClicked();
    void spinData();
private:
    Ui::MainWindow *ui;

	// widgets
	QGridLayout *ui_mainGridLayout;
	QLineEdit *ui_randomLine;
	QSpinBox *ui_lengthSpin;
	QComboBox *ui_algorithmCombo;
	QCheckBox *ui_lettersCheck;
	QCheckBox *ui_numbersCheck;
	QCheckBox *ui_punctCheck;
	QCheckBox *ui_customCheck;
	QStatusBar *ui_statusBar;
	QProgressBar *m_downloadingBar;
	QLineEdit *ui_customCharSetLine;

    QPushButton *ui_generateBtn;
    QPushButton *ui_appendBtn;
    QPushButton *ui_spinBtn;

	//networking
	QNetworkAccessManager *m_netManager;
	QNetworkReply *m_netReply;

	// Random/charset
	boost::mt19937 m_mTwister;
	QString m_charset;

    // Main functionality
    void genPass(int append);

    // Append to the current password
    bool m_append;
};

#endif // MAINWINDOW_H
