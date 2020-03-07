#include "worker.h"



Worker::Worker(QObject *parent) :
    QObject(parent)
{
}

Worker::~Worker()
{
    qDebug () << " Desctructor Called ";
    delete this->indexFile;
    delete this->manager;
}

Worker::Worker(DownloadListItem *dListItem, BlockInfo bi, MasterBufferManager *mbm)
{

    this->dListItem = dListItem;
    this->url = dListItem->currentUrl;
    this->bi = bi;
    this->mbm = mbm;


    QString indexFilePath = this->dListItem->segmentDir + QString::number( bi.index );
    this->indexFile = new QFile ( indexFilePath );

    if( !this->indexFile->open( QIODevice :: Append | QIODevice ::WriteOnly )) {
        qDebug () << " Cannot open Index File ";
    }

    this->indexFilePos = this->indexFile->pos();
    qDebug () << " index File Position = " << this->indexFilePos << "\n";

    /**

    QString indexFilePath = this->dListItem->segmentDir + QString::number( index );

    QFile *tmpFile = new QFile( indexFilePath );

    if( tmpFile->open(QIODevice::WriteOnly ) ) {

        tmpFile->write( buffer, size );
        tmpFile->flush();
        tmpFile->close();
        this->pendingBlocks.append( index );

    } else {
        qDebug () << " flush Buffer " << " Cannot Open File ";
    }

    if( tmpFile ) {
        delete tmpFile;
    }

   +-*/

    this->lastDownloadedBytes = 0;
    this->bufferSize = bi.end - bi.start;
    this->buffer = new char [ this->bufferSize + 1 ];
    this->stoppedFlag = false;

}



qint64 Worker::stop()
{
    qDebug () << " \n ============== WORKER STOP (" << this->bi.index << ") ============\n  " << __FILE__ << __LINE__;

    this->stoppedFlag = true;
    disconnect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT (finished (QNetworkReply *)));
    disconnect ( this->reply, SIGNAL(readyRead()), this, SLOT(readReady()));
    disconnect ( this->reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
    disconnect ( this->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    this->reply->abort();
    this->indexFile->flush();

    return 0;

}

void Worker::startWroking()
{

    this->manager =  new QNetworkAccessManager( this );

    QSslConfiguration cnf;
    cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
    cnf.setProtocol((QSsl::AnyProtocol));

    QNetworkRequest request( QUrl( this->url ));

    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
    request.setPriority( QNetworkRequest::HighPriority );
    request.setSslConfiguration(cnf);

    QString byteRange = "bytes=" + QString::number(bi.start + this->indexFilePos )  + QString("-") + QString::number(bi.end);

    request.setRawHeader(qStr2Ba("range"), qStr2Ba( byteRange ));

    this->reply = manager->get( request );

    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT (finished (QNetworkReply *)));
    connect ( this->reply, SIGNAL(readyRead()), this, SLOT(readReady()));

    connect ( this->reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
    connect ( this->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));


}

void Worker::progress(qint64 rcvd, qint64 total)
{   
    qint64 diff = rcvd - this->lastDownloadedBytes;
    this->lastDownloadedBytes = rcvd;
    emit updateProgress( diff );
}

void Worker::error(QNetworkReply::NetworkError err)
{
    qDebug () <<" Worker : Error ->  index = " <<  bi.index <<  " Error Occured " << this->reply->errorString() << __FILE__ << __LINE__;
}

void Worker::finished(QNetworkReply *rep)
{
    if( this->stoppedFlag == false ) {

        this->indexFile->flush();
        if( rep->error() == 0 ) {
           // this->mbm->flushBuffer( this->bi.index, this->buffer, this->bufferPos );
            this->mbm->flushBuffer( this->bi.index  );
            this->mbm->merge( this->bi.index );
        }

        if( this->buffer ) {
            delete this->buffer;
        }

        emit workFinished( bi, rep->error(), rep->errorString(), this );
        if( rep->error() == 0) {
            emit updateCube(  bi.index , 2 );
        } else {
            emit updateCube(  bi.index , 0 );
        }

    }

}

void Worker::readReady()
{
    qint64 readSize = this->reply->read( this->buffer , this->bufferSize );
    this->indexFile->write(this->buffer, readSize );
}

