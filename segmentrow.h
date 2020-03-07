#ifndef SEGMENTROW_H
#define SEGMENTROW_H

#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QTime>
#include <QProgressBar>
#include <QTableWidget>
#include "lib.h"
#include "helperclasses.h"
#include "filedownloader.h"



#define TRANSFER_ARR_SIZE 20

class SegmentRow : public QObject
{
    Q_OBJECT
public:
    explicit SegmentRow(QObject *parent = 0);
    SegmentRow(qint64 len);
    void clear();
    void updateProgress();


    /*======================================
     *     PROPERTIES
     ======================================*/


    qint64 totalSize;
    qint64 downloadedSize;

    /*=================================
     *      COMMONLY USED VARIABLES
    ===================================*/
    qint64 newSize;
    bool finishedFlag;
    QTime *dTime;
    int timeStart;
    int timeEnd;

    FileDownloader *fDownloader;


    QProgressBar *pb;
    QTableWidgetItem *lblTotalSize;
    QTableWidgetItem *lblDownloadedSize;
    QTableWidgetItem *lblTransferRate;

    int transferRateArr[ TRANSFER_ARR_SIZE ];
    int transferRate;
    int transferIndex;
    int transferAverageMax;

signals:
     void  downloadFinished();
    
public slots:
    void updateStatus(qint64 rcvd, qint64 total, int nCount);
    void finishedDownload(qint64 totalSize);

    
};

#endif // SEGMENTROW_H
