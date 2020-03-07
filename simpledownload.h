#ifndef SIMPLEDOWNLOAD_H
#define SIMPLEDOWNLOAD_H

#include <QObject>
#include <QtCore>

#include "masterbuffermanager.h"
#include "blockmanagementsystem.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "lib.h"


class SimpleDownload : public QObject
{
    Q_OBJECT
public:
    explicit SimpleDownload(QObject *parent = 0);
    void init ( DownloadListItem *dListItem );
    void startWorking ( );



    QNetworkAccessManager *manager;
    QNetworkReply *reply;


    DownloadListItem *dListItem;

    QFile *outputFile;

    qint64 rcvd;
    qint64 total;


signals:
    void downloadFinished ();
    void errorOccured( int errorCode, QString errorString );

public slots:
    void finished (QNetworkReply *rep);
    void readReady();
    void progress(qint64 rcvd,qint64 total);


};


/*


class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
    Worker ( QString url, BlockInfo bi, MasterBufferManager *mbm);

    QNetworkAccessManager *manager;
    QNetworkReply *reply;


signals:
    void updateProgress( qint64 );
    void updateCube( int index, int color);
    void workFinished();


public slots:
    void finished(QNetworkReply *rep);
    void readReady();
    void startWroking ();
    void progress(qint64 rcvd, qint64 total);

private:
    QString url;
    BlockInfo bi;
    MasterBufferManager *mbm;

    char * buffer;
    int bufferPos;
    qint64 bufferSize;
    qint64 lastDownloadedBytes;


};


*/

#endif // SIMPLEDOWNLOAD_H
