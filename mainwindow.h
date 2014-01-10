#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <random>

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

typedef std::mt19937 RngEngine;

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

    QProgressBar *m_downloadingBar;

    //networking
	QNetworkAccessManager *m_netManager;
	QNetworkReply *m_netReply;

	// Random/charset
    std::random_device m_rd;
    RngEngine m_rngEngine;

	QString m_charset;

    // Main functionality
    void genPass(int append);

    // Append to the current password
    bool m_append;
};

#endif // MAINWINDOW_H
