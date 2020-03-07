#include "analytics.h"

Analytics::Analytics(QObject *parent) :
    QObject(parent)
{


    QString filename = "";
    QDate d =  QDate::currentDate();
    filename = QString::number( d.year()) + "-" + QString::number( d.month()) + "-" + QString::number( d.day()) + ".log";
    this->currentFilename = filename;

    QString dir = "log";

    if( QDir().exists(dir) == false) {
        QDir().mkdir( dir );
    }

    QString fullPath = dir + "/" + filename;
    this->filepath = fullPath;


    //   file.open(stderr, QIODevice::WriteOnly);

    if( QFile().exists(fullPath) == false ) {

        this->formatFile( fullPath );
            qDebug () << " File Not Exists ";
        /*
        file = new QFile( fullPath );
        file->open( QIODevice::ReadWrite);
        formatFile();
        file->close();*/

    }



}

void Analytics::formatFile( QString filepath )
{
    usageData ud = {0};
    ud.totalDownloadCount = 0;
    this->file = new QFile( filepath );
    this->file->open( QIODevice ::WriteOnly )   ;
    QDataStream out( this->file );   // we will serialize the data into the file
    out.writeRawData( (char *)&ud, sizeof(ud));
    this->file->close();

}

void Analytics::debug()
{

    usageData ud = {0};
    this->file = new QFile( filepath );
    this->file->open( QIODevice :: ReadWrite ) ;
    QDataStream ds( this->file );   // we will serialize the data into the file
    ds.readRawData( (char *)&ud, sizeof(ud));
    this->file->close();

    qDebug () << " Total Download Count = " << ud.totalDownloadCount;
    qDebug () << " PR Count = " << ud.pauseResumeCount;
    qDebug () << " file Reties  Count = " << ud.fileRetiresCount;
    qDebug () << " download cancelled  Count = " << ud.downloadCancelledCount;

}

int Analytics::incDLCount()
{

    usageData ud = {0};
    this->file = new QFile( filepath );
    this->file->open( QIODevice :: ReadWrite ) ;
    QDataStream ds( this->file );   // we will serialize the data into the file
    int result = ds.readRawData( (char *)&ud, sizeof(ud));
    ud.totalDownloadCount ++;
    this->file->seek(0);
    ds.writeRawData( (char *)&ud, sizeof(ud));
    this->file->close();

}

int Analytics::incPRCount()
{

    usageData ud = {0};
    this->file = new QFile( filepath );
    this->file->open( QIODevice :: ReadWrite ) ;
    QDataStream ds( this->file );   // we will serialize the data into the file
    int result = ds.readRawData( (char *)&ud, sizeof(ud));
    ud.pauseResumeCount ++;
    this->file->seek(0);
    ds.writeRawData( (char *)&ud, sizeof(ud));
    this->file->close();

}

int Analytics::incFRCount()
{
    usageData ud = {0};
    this->file = new QFile( filepath );
    this->file->open( QIODevice :: ReadWrite ) ;
    QDataStream ds( this->file );   // we will serialize the data into the file
    int result = ds.readRawData( (char *)&ud, sizeof(ud));
    ud.fileRetiresCount ++;
    this->file->seek(0);
    ds.writeRawData( (char *)&ud, sizeof(ud));
    this->file->close();

}

int Analytics::incDCCount()
{

    usageData ud = {0};
    this->file = new QFile( filepath );
    this->file->open( QIODevice :: ReadWrite ) ;
    QDataStream ds( this->file );   // we will serialize the data into the file
    int result = ds.readRawData( (char *)&ud, sizeof(ud));
    ud.downloadCancelledCount ++;
    this->file->seek(0);
    ds.writeRawData( (char *)&ud, sizeof(ud));
    this->file->close();


}

QByteArray Analytics::getLogsJson()
{
    QJsonObject rootObj;
    QJsonArray logs;

    QDir dir("log");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString filename = fileInfo.fileName();

        if( filename != this->currentFilename ) {
            QJsonObject obj = readLogFile (  filename );
            logs.append( obj );
        }
    }
    rootObj["logs"] = logs;
    return QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

}


QJsonObject Analytics::readLogFile(QString filename)
{

      QJsonObject log;

      QString path = "log/" + filename;

      usageData ud = {0};
      this->file = new QFile( path );
      this->file->open( QIODevice :: ReadWrite ) ;
      QDataStream ds( this->file );   // we will serialize the data into the file
      ds.readRawData( (char *)&ud, sizeof(ud));
      this->file->close();
      QFile::remove( path );

      log["filename"] = filename;
      log["DL_count"] = QString::number(ud.totalDownloadCount);
      log["PR_count"] = QString::number(ud.pauseResumeCount);
      log["FR_count"] = QString::number(ud.fileRetiresCount);
      log["DC_count"] = QString::number(ud.downloadCancelledCount);



      return log;


}
