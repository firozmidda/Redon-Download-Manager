#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QMessageBox>
#include <QTime>
#include "downloadlist.h"
#include "systemdata.h"
#include "configsettings.h"
#include "lib.h"



namespace Ui {
class ConfirmDialog;
}

class ConfirmDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfirmDialog(QWidget *parent = 0);
    ~ConfirmDialog();
    void init(QString url, DownloadList *dList, SystemData *sysData , ConfigSettings *cSettings);
    void startProcess();
    void parseRawHeader(QNetworkReply *rep);
    QString getFilename(QNetworkReply *rep);
    int checkFileType(QString ext);
    void finished();
    void displayLoader ();
    void hideLoader ();

    void processFileExistance ();
    QString generateNewFilename ( QString path, QString filenameOnly, QString ext );
    void fixPath();




signals:
    void downloadConfirmed( DownloadListItem *dListLiem);


public slots:
    void lengthReceived (QNetworkReply *rep);
    void readReady ( );
    void error(QNetworkReply::NetworkError err);
    void sslError(QList<QSslError>);

    
private slots:
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_3_clicked();
    void on_btn_start_clicked();

private:
    Ui::ConfirmDialog *ui;
    QString url;
    DownloadList *dList;
    SystemData *sysData;
    ConfigSettings *cSettings;
    DownloadListItem *dListItem;


    /** ========================== information variable ====================*/
    QString downloadPath;
    QFileDialog *dlg;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QMovie *movie;

    /** ========================== Other Variables   =============================*/
    QString localFilename;
    QString pathOnly;
    int speedCheckArray[10];
    int speedCheckIndex;
    QTime *speedTimer;



};

#endif // CONFIRMDIALOG_H
