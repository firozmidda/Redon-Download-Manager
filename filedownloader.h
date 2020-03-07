#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H


#include <QtCore>
#include <QtGui>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSsl>
#include <QSslConfiguration>
#include <QFile>

#include "helperclasses.h"
#include "downloadlist.h"
#include "filedownloader.h"
#include "lib.h"


class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QObject *parent = 0); 
    FileDownloader(DownloadListItem *dListItem, segmentInfo si, int n, DownloadState *dState);
    void simpleDownload();
    void multipartDownload();
    void bindConnection();
    void unbindConnection();
    void stop();


signals:
    void updateStatus(qint64 rcvd, qint64 total, int nCount);
    void finished(qint64 size );
    
public slots:
    void finishDownload(QNetworkReply *rep);
    void error(QNetworkReply::NetworkError err);
    void progress(qint64 rcvd, qint64 total);
    void readReady();
    void start();


private:

    DownloadListItem *dListItem;
    segmentInfo si;


    int nCount;
    QFile *stateFile;
    QFile *file;
    QString tmpDir;
    bool started;
    QTextStream *textStream;
    DownloadState *dState;


/*
    DownloadInfo dInfo;
    bool multipartFlag;
    qint64 receivedSize;
    qint64 totalSize;
    qint64 rangeStart;
    qint64 rangeEnd;


    qint64 currentRangeStart;
    qint64 lastReceivedSize;
    qint64 lastTotal;

    int nCount;
    QFile *stateFile;
    QFile *file;
    QFile *tmpFile;
    QString tmpDir;
    QTimer *timer;
    bool started;

*/
    // size
    qint64 initial;
    qint64 receivedSize;
    qint64 totalSize;
    qint64 lastReceivedSize;
    qint64 lastTotal;


    /*===========================================
     *      NETWORK
    ===============================================*/

    QNetworkAccessManager *manager;
    QNetworkReply *reply;
};

#endif // FILEDOWNLOADER_H
