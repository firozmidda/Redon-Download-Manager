#ifndef MASTERBUFFERMANAGER_H
#define MASTERBUFFERMANAGER_H

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include <QtCore>
#include "downloadlist.h"


class MasterBufferManager
{
public:
    MasterBufferManager(DownloadListItem *dListItem);
    int init();
    int stop();
    void flushBuffer( int index);
    bool merge( int index );
    void finish();
    int appendFile(int index);
    void readMergeLog();
    void saveMergeLog();
    qint64 bytesDownloaded ( int blockSize);

    QList<int> indexList;
    QList<int> pendingBlocks;

    DownloadListItem *dListItem;
    QFile *logFile;
    QFile *mergeFile;
    QFile *outputFile;

    QString logFilePath;
    QString mergeFilePath;
    QString outputFilePath;

    int mergeIndex;

};

#endif // MASTERBUFFERMANAGER_H
