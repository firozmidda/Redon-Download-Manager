#include "filedownloader.h"
#include <QDebug>

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent)
{
}

FileDownloader::FileDownloader(DownloadListItem *dListItem, segmentInfo si, int n, DownloadState *dState)
{
    this->dListItem = dListItem;
    this->si = si;
    //this->textStream = textStream;
    this->dState = dState;
    this->nCount = n;
    this->tmpDir = this->dListItem->segmentDir;
    this->initial = 0;
    this->initial = si.current;

    this->lastReceivedSize = 0;
    this->lastTotal = 0;

    this->totalSize = 0;
    this->manager = NULL;
    this->reply = NULL;


    if( this->dListItem->multipart == true ) {
        this->file = new QFile( this->tmpDir + "/" + "part" + QString::number(this->nCount) + ".part" );
    } else {
        this->file = new QFile( this->dListItem->downloadRoot + "/" + this->dListItem->filename );
    }

    if( this->file->open( QIODevice :: Append ) == false ) {
       return;
    }

    /*
    this->dInfo = dInfo;
    this->multipartFlag = multipart;
    this->rangeStart = rangeStart;
    this->currentRangeStart = rangeStart;
    this->rangeEnd = rangeEnd;
    this->nCount = nCount;
    this->tmpDir = tmpDir;
    this->lastReceivedSize = 0;
    this->lastTotal = 0;
    this->started = false;
    this->timer = new QTimer();
    this->stateFile = stateFile;

    this->initialRange = rangeStart;
    this->currentRange = rangeStart;
    this->endRange = rangeEnd;

    if( this->multipartFlag  == true ) {
        this->file = new QFile( this->tmpDir + "/" + "part" + QString::number(nCount) + ".part");
        if( this->file->open( QIODevice :: WriteOnly) == false ) {
            return;
        }

    } else {
        QString path = dInfo.getDownloadRoot(false);
        path = path + "/" + dInfo.filename;
        this->file = new QFile( path );
         if( this->file->open( QIODevice :: WriteOnly) == false ) {
            return;
        }
    }



*/

}

void FileDownloader::simpleDownload()
{

    QSslConfiguration cnf;
    cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
    cnf.setProtocol((QSsl::AnyProtocol));

    this->manager = new QNetworkAccessManager();
    QNetworkRequest request ( this->dListItem->currentUrl );
    request.setSslConfiguration(cnf);


    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
    request.setPriority( QNetworkRequest::HighPriority );


    this->reply = this->manager->get(request);
    this->bindConnection();

}

void FileDownloader::multipartDownload()
{
    qDebug () << " Initail total Size = " << this->totalSize;
    int diff = this->si.end - this->si.current;
    qDebug () << " diff = " << diff;
    if( diff >  0 ) {

        QSslConfiguration cnf;
        cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
        cnf.setProtocol((QSsl::AnyProtocol));

        this->manager = new QNetworkAccessManager();
        QNetworkRequest request( this->dListItem->currentUrl );
        request.setSslConfiguration(cnf);


        request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
        request.setPriority( QNetworkRequest::HighPriority );

        QString rangeString = "bytes=" + QString::number( this->si.current ) + "-" + QString::number( this->si.end );
      //  qDebug () << "range String  = " << rangeString;
        request.setRawHeader(qStr2Ba("range"), qStr2Ba(rangeString));
        this->reply = this->manager->get(request);
       // qDebug () << " binding connection ";
        this->bindConnection();

    } else {

        progress( 0, this->si.end);
        qDebug () << " last total Size = " << this->totalSize;
        emit finished( this->totalSize );

    }


}

void FileDownloader::bindConnection()
{

    connect ( this->manager, SIGNAL(finished(QNetworkReply*)), this,SLOT(finishDownload(QNetworkReply *)));
    connect ( this->reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(error(QNetworkReply::NetworkError)));
    connect ( this->reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(progress(qint64, qint64)));
    connect ( this->reply, SIGNAL(readyRead()), SLOT(readReady()));

}

void FileDownloader::unbindConnection()
{

    if( this->manager ) {

        disconnect ( this->manager, SIGNAL(finished(QNetworkReply*)), this,SLOT(finishDownload(QNetworkReply *)));
        disconnect ( this->reply, SIGNAL(error(QNetworkReply::NetworkError)),this,  SLOT(error(QNetworkReply::NetworkError)));
        disconnect ( this->reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64, qint64)));
        disconnect ( this->reply, SIGNAL(readyRead()),this,  SLOT(readReady()));

    }

}

void FileDownloader::stop()
{

   this->unbindConnection();
   if( this->reply ) {
       this->reply->abort();
   }

   if( this->file ) {
       this->file->flush();
       this->file->close();

   }


}

void FileDownloader::start()
{

    if( this->dListItem->multipart == false ) {
        this->simpleDownload();
    } else {
        this->multipartDownload();
    }
}

void FileDownloader::finishDownload(QNetworkReply *rep)
{

    Q_UNUSED (rep);
    this->file->flush();
    this->file->close();
    emit finished( this->totalSize );
}

void FileDownloader::error(QNetworkReply::NetworkError err)
{
  qDebug() << " error code = " << err ;
}

void FileDownloader::progress(qint64 rcvd, qint64 total)
{

    /*
    qDebug () << this->nCount << "  "<< rcvd - this->lastReceivedSize;
    this->lastReceivedSize = rcvd;
    this->receivedSize = rcvd;
    this->totalSize = total;

    this->si.current = this->initial + rcvd;

    */
  //  qDebug () << " Progress  rcvd = " << rcvd << "  total = " << total;

    qint64 receivedBytes = (this->si.current - this->si.initial );
    qint64 totalBytes = ( this->si.end - this->si.initial );
    emit updateStatus( receivedBytes , totalBytes , this->nCount );
    if( rcvd > 0 ) {
        this->totalSize = total;
    } else {
        this->totalSize = totalBytes;
    }


}

void FileDownloader::readReady()
{

   // QTime benchmark;
    //benchmark.start();

    QMutex mutex;
    mutex.lock();

    QByteArray tmp = this->reply->readAll();
    qint64 rcvd = tmp.size();

    this->file->write( tmp  );

    this->lastTotal += rcvd;

    this->si.current += rcvd;


    if( this->dListItem->multipart == true ) {
        this->dState->writeSegment( this->nCount, this->si.current );
    }

    mutex.unlock();
   // qDebug () << " Read Ready ends = " << benchmark.elapsed();

}
