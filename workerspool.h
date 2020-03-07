#ifndef WORKERSPOOL_H
#define WORKERSPOOL_H

#include <QObject>
#include <QDebug>
#include <QThreadPool>
#include "blockmanagementsystem.h"
#include "masterbuffermanager.h"
#include "worker.h"
#include "downloadlist.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


#define MAX_TRANSFER_LEN 20

#define MAX_TICK  ((_SPEED / 1024 ) * 1)
#define TIMESTAMP_DURATION   ( _MAXCONNECTION * (MAX_BLOCK_SIZE / _SPEED  ) / 2)


class WorkersPool : public QObject
{

    Q_OBJECT
public:
    explicit WorkersPool(QObject *parent = 0);
    WorkersPool( DownloadListItem *dListItem , BlockManagementSystem *bms, MasterBufferManager *mbm, int maxConnection);
    void startWorking();
    void createWork ( BlockInfo bi );
    void debug ();
    void stop ();
    void removePendingList ( BlockInfo bi);
    void removeFinishedIndex ( int index );
    void stopPendingBlock( BlockInfo bi);
    void closeThread( BlockInfo bi);

    void setTickValue ( int value ) ;

    void addTransferRate( int rate);
    qint64 getBytesDownloaded( );


    QList<Worker *> workerList;
    QList<Worker *> deleteWorker;

    int totalWorkers;
    int maxWorkers;
    int currentMaxWorker;

    bool stoppedFlag;
    bool downloadCompleted;
    QString url;
    DownloadListItem *dListItem;


    qint64 bytesDownloaded;
    int tick;
    bool workFinishedFlag;
    QList<BlockInfo> pendingBlocks;
    int maxTicks;
    int errorCount;

signals:
    void updateCube (  int index, int color );
    void errorOccured( int errorCode, QString errorString );
    void downloadFinished();

public slots:
    void finishedWork (BlockInfo bi, int errorCode, QString errorString, Worker *w);
    void updateTransferRate(qint64 rcvd);


private:
    BlockManagementSystem *bms;
    MasterBufferManager *mbm;


};

#endif // WORKERSPOOL_H
