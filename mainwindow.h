#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QTreeWidgetItem>
#include <QNetworkInterface>
#include <stdlib.h>
#include <QSystemTrayIcon>


#include "systemdata.h"
#include "downloadlist.h"
#include "configsettings.h"
#include "urldialog.h"
#include "videodialog.h"
#include "newdownloader.h"
#include "propertydialog.h"
#include "helperclasses.h"
#include "aboutdialog.h"
#include "popupdialog.h"
#include "analytics.h"


#include <redonserver.h>


#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


#ifdef QT_DEBUG
    #define REDON_URL "http://127.0.0.1/home/products/redon/"
#else
    #define REDON_URL "http://www.diodecode.com/products/redon/"
#endif

// api.diodecode.com/redon/analytics/
// dc_api/redon/analytics/

#ifdef QT_DEBUG
    #define REDON_URL "http://127.0.0.1/api_dc/redon/analytics/"
#else
    #define REDON_URL "http://api.diodecode.com/redon/analytics/"    
#endif




/*=============================
 *  WINDOWS TO INCLUDE
===============================*/

#include "settingsdialog.h"


class SingleApplicationListener: public QObject {
    Q_OBJECT

signals:
    void restore();
    void downloadFromUrl(QString option, QString url);

public slots:
    void lintener() {

        QSharedMemory sharedMemory("singleApplication");
        while ( 1 ) {

            sharedMemory.attach();
            sharedMemory.lock();
            char *to = (char*)sharedMemory.data();

            if (to != 0 ) {

                QString str( to);
                if( str.contains("activate")) {
                    sharedMemory.detach();
                    emit  restore();
                } else if( str.contains ("url")) {
                    sharedMemory.detach();
                    emit  downloadFromUrl( "url",  str.split(" ")[1]);
                } else if( str.contains("video")) {
                    sharedMemory.detach();
                    emit  downloadFromUrl( "video",  str.split(" ")[1]);
                }

            }
            sharedMemory.unlock();
            Sleep( 500 );

        }

    }

};



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init(QString option, QString optionValue);
    void setupTree();
    void addTreeRoot(QString name, QIcon icon);
    void addTreeChild(QTreeWidgetItem *parent, QString name, QIcon icon);
    void setupList();
    void setupListHeader();
    void loadList();

    void addRowItem(int i, int j);
    void refereshList();

    void setupSystemTrayIcon();


    QString getMacAddress ();
    QString mixIt(QString ApplicationVersion, QString ReleaseType);
    void checkUpdate();

    QSqlDatabase listDB;


    Analytics *redonUsageData;

    SystemData *sysData;
    DownloadList *dList;
    ConfigSettings *cSettings;
    NewDownloader *nDownloader;
    QStandardItemModel *model;
    QSystemTrayIcon *sTrayIcon;
    bool hideWindowFlag;


    SingleApplicationListener *sap;
    QThread *sapThread;


    /*==================================
     *  DIALOG WINDOWS
     ====================================*/
    SettingsDialog *sDialog;
    UrlDialog *uDialog;
    VideoDialog *vDialog;
    PropertyDialog *pDialog;
    AboutDialog *aDialog;

    /*========================================
     *  FOR CHECK UPDATE
    =========================================*/
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    RedonServer *redonServer;



public slots:
    //void doDownload(QString str);
    void doDownload(DownloadListItem dListItem );
    void createModelRow( TableModel *tModel);
    void tblDoubleClick(QModelIndex model);
    void customMenuRequested(QPoint pos);
    void treeWidgetClicked(QTreeWidgetItem *treeWidget,int i);
    void modelResetEvent();
    void refereshDownloadList();
    void activateMainWindow(QSystemTrayIcon::ActivationReason reason);

    void downloadFromUrl(QString option, QString url);
    void gotRequest( QString str);

    void exit();


    /** *==============================================
     *  REPLY FINISHED
     *
    ===============================================*/
    void replyFinished(QNetworkReply *rep);

    /** ===========================================
     *      CONTEXT MENU SLOTS
     *===============================================*/
    void openMenuAction();
    void openFolderAction();
    void propertyDialog();
    void resumeAction ();
    void removeFromList();



private slots:
    void on_actionNew_Download_triggered();
    void on_actionVideo_Download_triggered();
    void on_actionOptions_triggered();
    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void closeEvent(QCloseEvent *event);

    void on_actionCheck_Updates_triggered();

private:
    Ui::MainWindow *ui;
    HelperClasses *hc;

    int contextMenuRow;


};

#endif // MAINWINDOW_H
