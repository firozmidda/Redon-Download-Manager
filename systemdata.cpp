#include "systemdata.h"



SystemData::SystemData()
{
    this->errorFlag = false;
    this->redonUsageData = new Analytics();
}

int SystemData::checkDefaultDownloadPath()
{
    this->homePath = QDir::homePath();
    QString downloadPath = "";
    QString directoryName = "Downloads";


    if( QDir(this->homePath).exists()) {
        downloadPath = homePath;

        if( QDir ( downloadPath + "/"+ directoryName +"/" ).exists()) {
            downloadPath = downloadPath + "/"+ directoryName +"/" ;
        } else {

            if( QDir().mkdir( downloadPath + "/"+ directoryName +"/"  ) ) {
                 downloadPath = downloadPath + "/"+ directoryName +"/" ;
            }
        }

    }
    this->defaultDownloadPath = downloadPath;
    if( downloadPath == "") {
        return -1;
    } else {
        return 0;
    }

}

int SystemData::checkDownloadDirs(DownloadList *dList)
{
    QString path = this->defaultDownloadPath;
    for( int i = DownloadList::GENERAL ; i < DownloadList::CUSTOM ; i ++ ) {
        QString tmpPath = path + dList->categoryString[i];
        if( !QDir ( tmpPath ).exists()) {
              QDir().mkdir( tmpPath );
        }
    }
    return 0;
}

int SystemData::checkTmpDir()
{
    this->tmpRootDir = QDir::tempPath();
    if ( QDir (this->tmpRootDir).exists()) {
        this->tmpRootDir = this->tmpRootDir + "/redon/";
        QDir().mkdir( this->tmpRootDir);
    }
    return 0;
}

int SystemData::loadMimeData(QString filename)
{
    QFile file( filename);

    fileDebug (" Trying TO open Mime FIle \n"  );
    if ( file.open(QIODevice::ReadOnly)) {

         fileDebug (" mime File Opened \n"  );
          QTextStream stream ( &file );
          while ( !stream.atEnd()) {
              QString line = stream.readLine();
              QStringList arr =  line.split(":");
              QString key = arr[0];
              QString value = arr[1];
              this->mimeType.insert( key, value )  ;
          }

    } else {
        fileDebug (" cannot Open The Mime File \n") ;
        return -1;
    }
    fileDebug (" Returning From loadMimeData  \n"  );
    return 0;
}

QString SystemData::getExtension(QString contentType)
{
    int end = contentType.indexOf(';');
    QString type = contentType.mid(0, end);
    type = type.trimmed();
    return this->mimeType.value( type );
}
