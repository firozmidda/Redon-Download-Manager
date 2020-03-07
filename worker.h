#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QtCore>
#include <QRunnable>

#include "blockmanagementsystem.h"
#include "masterbuffermanager.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "lib.h"


class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
    ~Worker ();
    Worker (DownloadListItem *dListItem, BlockInfo bi, MasterBufferManager *mbm);

    qint64 stop ();

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    BlockInfo bi;
    bool stoppedFlag;

signals:
    void updateProgress( qint64 );
    void updateCube( int index, int color);
    void workFinished( BlockInfo bi, int errorCode, QString errorString, Worker *w);



public slots:
    void finished(QNetworkReply *rep);
    void readReady();
    void startWroking ();
    void progress(qint64 rcvd, qint64 total);
    void error(QNetworkReply::NetworkError err);



private:
    DownloadListItem *dListItem;
    QString url;
    QFile *indexFile;
    MasterBufferManager *mbm;
    int indexFilePos;


    char * buffer;
    qint64 bufferSize;
    qint64 lastDownloadedBytes;


};

#endif // WORKER_H
