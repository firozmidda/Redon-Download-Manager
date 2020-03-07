#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "systemdata.h"
#include "downloadlist.h"
#include "lib.h"
#include "configsettings.h"
#include "helperclasses.h"
#include "segmentrow.h"
#include "filejoiner.h"
#include "blockmanagementsystem.h"
#include "masterbuffermanager.h"
#include "workerspool.h"
#include "simpledownload.h"

#define MAX_STABILIZER 10

namespace Ui {
class DownloadDialog;
}

class DownloadDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DownloadDialog(QWidget *parent = 0);
    ~DownloadDialog();
    void init( DownloadListItem *dListItem, DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings);
    void resume( DownloadListItem *dListItem, DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings);
    void getSegmentInfo();
    void newSegmentInfo();
    void readSegmentInfo();
    void setupDialog();
    void drawTable();


    void setupCubes();
    void drawCubes();


    void start();
    void stop();

    void multipartDownload();
    void simpleDownload();
    void addTransferRate(qint64 rcvd );

    QThread::Priority getThreadPriority(int i);




signals:
    void downloadCompleted ();
    void downloadStopped();


public slots:
    void multipartUpdateStatus();
    void simpleUpdateStatus();
    void downloadFinished();
    void closeEvent(QCloseEvent *event);
    void updateCube (int index, int color );
    void updateStatus();
    void errorOccured(int errorCode, QString errorString);



private slots:
    void on_btn_pause_clicked();

    void on_btn_resume_clicked();



    void on_btn_play_clicked();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::DownloadDialog *ui;
    DownloadListItem *dListItem;
    DownloadList *dList;
    SystemData *sysData;
    ConfigSettings *cSettings;
    QTimer *timer;
    QTime *downloadTime;
    bool stoppedFlag;
    bool downloadCompletedFlag;
    bool canPlayFlag;


    /*============================================
     *      BLOCK MANAGEMENT SYSTEM
    ============================================*/
    BlockManagementSystem *bms;
    MasterBufferManager *mbm;
    WorkersPool *wp;

    int cubeDivider;
    QGraphicsPixmapItem *cubeItem[ MAX_CUBES ];
    QGraphicsScene *scene;
    int maxBlocks;

    qint64 lastDownloadedBytes, downloadedBytes, lastTimeMsec;
    qint64 previousDownloadedBytes;
    qint64 stabilizerArray[ MAX_STABILIZER];
    int stabilizerIndex, stabilizerMax, stabilizerTick;

    /*===============================================
     *         SIMPLE DOWNLOAD
     ================================================*/
    SimpleDownload *sd;



    /*================================================
     *      P R O G R A M   V A R I A B L E S
      ===============================================*/

    QString tmpDir;
    segmentInfo segmentInfoArr[11];
    int seginfoIndex;
    //TableModel *tModel;
    int maxConnection;
    SegmentRow *sRow[11];
    DownloadState *dstate;

    /*==========================================
     *      Ui Variables
    ===========================================*/

    qint64 downloadedSize;
    qint64 totalSize;
    int transferRate;




};

#endif // DOWNLOADDIALOG_H
