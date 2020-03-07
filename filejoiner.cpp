#include "filejoiner.h"

FileJoiner::FileJoiner(QObject *parent) :
    QObject(parent)
{
}

FileJoiner::FileJoiner(DownloadListItem *dListItem, int maxConnection)
{

    this->dListItem = dListItem;
    this->maxConnection = maxConnection;

    this->file = new QFile(this->dListItem->downloadRoot + "/" + this->dListItem->filename );
    if( this->file->open( QIODevice::WriteOnly)) {
        this->joinFile();
    }

}

void FileJoiner::joinFile()
{

    QFile *tmpFile;

    for( int i = 0 ; i < this->maxConnection; i ++ ) {
        QString path = this->dListItem->segmentDir + "part" + QString::number(i) + ".part";
        tmpFile = new QFile(path);
        if( tmpFile->open(QIODevice::ReadOnly) ) {
            this->file->write( tmpFile->readAll()  );
            tmpFile->close();
        }
    }
    this->file->flush();
    this->file->close();

    QDir qdir( this->dListItem->segmentDir );
    qdir.removeRecursively();


}
