#ifndef BLOCKMANAGEMENTSYSTEM_H
#define BLOCKMANAGEMENTSYSTEM_H

#include <QtCore>
#include <QTextStream>
#include "downloadlist.h"


#define MAX_CUBES 10000

struct BlockInfo {
    qint64 start;
    qint64 end;
    qint64 index;
    qint64 timestamp;
};



class BlockManagementSystem
{

    qint64 filesize;
    int blockSize;
    int divider;
    qint64 start, end, index;
    DownloadListItem *dListItem;


public:




    BlockManagementSystem(DownloadListItem *dListItem, qint64 filesize );
    void next();
    void stop();
    void calculateBlockSize();
    BlockInfo getBlock();
    int readStateLog();
    bool isCompleted(int index);
    inline qint64 getCurrentIndex() { return this->index; }

    inline int getBlockSize () { return this->blockSize; }
    inline int getDivider () { return this->divider; }
    QList<int> completedBlockList;


};

#endif // BLOCKMANAGEMENTSYSTEM_H

