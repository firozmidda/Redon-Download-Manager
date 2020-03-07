#include "helperclasses.h"

#include <QDebug>

HelperClasses::HelperClasses()
{
}

void HelperClasses::openFolder(QString filepath)
{
    process = new QProcess();
    filepath =  QDir::toNativeSeparators( filepath );
    QFile file(filepath);
    if( !file.exists() ) {
        filepath = QFileInfo(filepath).absolutePath();
    }

    process->startDetached(" Explorer /select, " + filepath );


}


TableModel::TableModel(QString url, QString filename, QIcon icon)
{

    QIcon fileIcon;

    if( icon.isNull() ) {
        fileIcon.addFile( ":/menu_icon/icon/dicon.png");
    } else{
        fileIcon = icon;
    }

    //this->icon = new QStandardItem( QIcon(":/menu_icon/icon/dicon.png"), "" );
    this->filename = new QStandardItem( fileIcon, filename );
    this->status = new QStandardItem( QString (""));
    this->size = new QStandardItem( QString (""));
    this->transferRate = new QStandardItem( QString (""));
    this->timeLeft = new QStandardItem( QString (""));
    this->savedPath = new QStandardItem( QString (""));
    this->url = new QStandardItem( url );
}

void TableModel::setTimeLeft(QString timeleft)
{
    if( this->timeLeft->model()) {
        this->timeLeft->setText( timeleft );
    }

}

QStandardItem *TableModel::getTimeLeft()
{
    return this->timeLeft;
}



/*======================================================
 *              D o w n lo a d   S t a t e
========================================================*/

DownloadState::DownloadState(QString filename)
{
    this->filename = filename;
    this->stateFile = new QFile( this->filename );
    this->tmpFile = NULL;

    this->textStream = NULL;
    this->tmpStream = NULL;
}

void DownloadState::open()
{

    this->stateFile->open( QIODevice ::ReadWrite );
    this->textStream = new QTextStream ( this->stateFile );

}


void DownloadState::flush()
{
    if( this->textStream ) {
        this->textStream->flush();
    }

}

void DownloadState::close()
{
    this->flush();
    this->stateFile->close();
}

void DownloadState::init(segmentInfo si[], int len)
{
    *this->textStream << len << ";\n";
    for( int i  = 0 ; i < len; i ++) {
        this->writeFullSegmentInfo(i, si[i].initial, si[i].current, si[i].end );
    }
    this->textStream->flush();
}

void DownloadState::readSegment(segmentInfo si[], int *len)
{
    int maxSegment = 0;

    if( this->stateFile->isOpen()) {
        this->stateFile->flush();
        this->stateFile->close();
    }


    this->tmpFile = new QFile( this->filename );
    if( this->tmpFile->open( QIODevice::ReadWrite) ) {
        this->tmpStream = new QTextStream(this->tmpFile);

        QString line = this->tmpStream->readLine().replace(";", "");
        maxSegment = line.toInt();
        qDebug () << " max  segment " << maxSegment;

        for( int i= 0 ; i < maxSegment; i ++) {
            line = this->tmpStream->readLine().replace(";", "");
            QStringList arr = line.split(":");

            int index = arr[0].toInt();
            si[ index ].initial = arr[1].toLongLong();
            si[ index ].current = arr[2].toLongLong();
            si[ index ].end = arr[3].toLongLong();
        }

        while ( !this->tmpStream->atEnd()) {
            line = this->tmpStream->readLine().replace(";", "");
            QStringList arr = line.split(":");
            int index = arr[0].toInt();
            si[ index ].current = arr[1].toLongLong();

        }

        this->tmpFile->close();
    }
    *len = maxSegment;

}

void DownloadState::writeSegment(int index, qint64 current)
{
    *this->textStream << index << ":" << current <<";\n";
}

void DownloadState::writeFullSegmentInfo(int index, qint64 initial, qint64 current, qint64 end)
{
    *this->textStream << index << ":" << initial << ":" << current << ":" << end <<";\n";
}

void DownloadState::reopen()
{
    bool flag = true;

    if( !this->stateFile->open( QIODevice::ReadWrite ) ) {
        flag = false;
    }

    if( this->stateFile->size() > 0 ) {
        this->stateFile->seek( this->stateFile->size() );
    }

    this->textStream = new QTextStream( this->stateFile );


}
