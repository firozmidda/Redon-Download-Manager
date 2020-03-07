#include "simpledownload.h"

SimpleDownload::SimpleDownload(QObject *parent) :
    QObject(parent)
{

}

void SimpleDownload::init(DownloadListItem *dListItem)
{

    this->dListItem = dListItem;
    this->total = 0;
    this->rcvd = 0;

    QString outputFilePath = this->dListItem->downloadRoot + this->dListItem->filename;

    this->outputFile = new QFile( outputFilePath );
    if( this->outputFile->open(QIODevice::WriteOnly )) {
        this->startWorking();
    } else {
        qDebug () << " Simple Download Error : Cannot Open File ";
    }

}

void SimpleDownload::startWorking()
{

    this->manager =  new QNetworkAccessManager( this );

    QSslConfiguration cnf;
    cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
    cnf.setProtocol((QSsl::AnyProtocol));

    QNetworkRequest request( QUrl( this->dListItem->currentUrl ));
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
    request.setPriority( QNetworkRequest ::HighPriority );
    request.setSslConfiguration(cnf);

    this->reply = manager->get( request );

    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT (finished (QNetworkReply *)));
    connect ( this->reply, SIGNAL(readyRead()), this, SLOT(readReady()));
    connect ( this->reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));

}

void SimpleDownload::finished(QNetworkReply *rep)
{

    if( rep->error() == 0 ) {
        this->outputFile->flush();
        this->outputFile->close();
        emit downloadFinished();

    } else {
        errorOccured( rep->error(), rep->errorString());
    }

}


void SimpleDownload::readReady()
{
    this->outputFile->write( this->reply->readAll() );
}

void SimpleDownload::progress(qint64 rcvd, qint64 total)
{    
    QMutex mutex;
    mutex.lock();
    this->rcvd = rcvd;
    this->total = total;
    mutex.unlock();
}
