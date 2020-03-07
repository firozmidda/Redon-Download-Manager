#include "blockmanagementsystem.h"

BlockManagementSystem::BlockManagementSystem(DownloadListItem *dListItem, qint64 filesize)
{
    this->dListItem = dListItem;
    this->filesize = filesize;

    this->divider = 1;
    this->index = 1;
    this->start = 0;
    this->calculateBlockSize();
    this->end = this->blockSize;
    this->readStateLog();

    qint64 totalBlocks = filesize / this->blockSize;

    this->divider = totalBlocks / MAX_CUBES; /// Max Total Cube
    this->divider ++;

}

void BlockManagementSystem::next()
{
    this->index ++;
    this->start = this->end + 1;
    this->end += this->blockSize;

}

void BlockManagementSystem::stop()
{
    qDebug () << " BMS  stop ";

}

void BlockManagementSystem::calculateBlockSize()
{

    int maxConnection = 2;

    qint64 halfSize = this->filesize / 4 ;

    this->blockSize = halfSize / maxConnection;


    if( this->blockSize > MAX_BLOCK_SIZE ) {
        this->blockSize = MAX_BLOCK_SIZE;
    }

    if( this->blockSize < MIN_BLOCK_SIZE) {
        this->blockSize = MIN_BLOCK_SIZE;
    }


    qDebug() << " Block Size = " << this->blockSize / 1024 <<"KB ";



}

BlockInfo BlockManagementSystem::getBlock()
{
    BlockInfo bi;


    while( this->isCompleted( this->index )) {
        next();
        if( this->start >= this->filesize ) {
            break;
        }
    }


    if( this->end > this->filesize ) {
        this->end = this->filesize;
    }

    if( this->start >= this->filesize ) {
        this->end = 0;
    }


    bi.start = this->start;
    bi.end = this->end;
    bi.index = this->index;
    bi.timestamp = QDateTime::currentMSecsSinceEpoch();

    this->next();

    return bi;
}

int BlockManagementSystem::readStateLog()
{


    QString logFilePath = this->dListItem->segmentDir + "state.log";

    qDebug() << " ReadState Log = ";
    QFile *logFile = new QFile( logFilePath );
    if( logFile->open( QIODevice::ReadOnly ) ) {
        QTextStream st(logFile);
        while ( !st.atEnd())  {
            this->completedBlockList.append( st.readLine().toInt()  );
        }
    }
    qSort( this->completedBlockList.begin(), this->completedBlockList.end() );
    qDebug () << " Read State Log " <<  this->completedBlockList;

}

bool BlockManagementSystem::isCompleted(int index)
{
    // later we will use binary search to find
    return this->completedBlockList.contains( index );
}

