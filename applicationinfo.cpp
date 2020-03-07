#include "applicationinfo.h"



ApplicationInfo::ApplicationInfo()
{
    this->updateStatusFlag = false;
    this->updateFilePath = "";
    this->appInfoFilename = "appInfo.xml";

    this->checkAndCreateAppInfo();
    this->readAppInfo();
    this->processAppInfo();

}

void ApplicationInfo::checkAndCreateAppInfo()
{

    QFile appFile( this->appInfoFilename );
    if( !appFile.exists() ) {

        if( !appFile.open( QIODevice::WriteOnly )) {
            return;
        }

        QDomDocument document;
        QDomElement root = document.createElement("ApplicationInfo");
        document.appendChild( root );

        QDomElement buildVersionNode = document.createElement("build_version");
        buildVersionNode.setAttribute("value", BUILD_VER );
        root.appendChild( buildVersionNode );


        QDomElement applicationVersionNode = document.createElement("application_version");
        applicationVersionNode.setAttribute("value", APPLICATION_VERSION );
        root.appendChild( applicationVersionNode );

        QDomElement signatureNode = document.createElement("signature");
        signatureNode.setAttribute("value", REDON_SIGNATURE );
        root.appendChild( signatureNode );

        QDomElement updateStatusNode = document.createElement("update_status");
        updateStatusNode.setAttribute("value", this->updateStatusFlag );
        root.appendChild( updateStatusNode );

        QDomElement updateFilePathNode = document.createElement("update_file_path");
        updateFilePathNode.setAttribute("value", this->updateFilePath );
        root.appendChild( updateFilePathNode );

        QTextStream stream ( &appFile );
        stream <<document.toString();
        appFile.close();

        qDebug () << " File Created ";

    }
}

void ApplicationInfo::readAppInfo()
{
    QFile appFile( this->appInfoFilename );
    if( !appFile.open( QIODevice::ReadOnly )) {
        qDebug () << " Cannot Open Appinfo File ";
        return ;
    }


    QDomDocument document;
    document.setContent( &appFile );

    QDomElement root = document.firstChildElement();

    if( !root.isNull() ) {

        this->updateStatusFlag = root.elementsByTagName("update_status").at(0).toElement().attribute("value").toInt();
        this->updateFilePath = root.elementsByTagName("update_file_path").at(0).toElement().attribute("value");

    } else {
        qDebug () << " Invalid Appinfo File ";
        // delete and create new One
    }

    qDebug () << " update Status = " << this->updateStatusFlag;
    qDebug () << " upadet file path = " << this->updateFilePath;

}

void ApplicationInfo::processAppInfo()
{

    fileDebug( " In Process Info  ");

    if( this->updateStatusFlag == true ) {

        fileDebug (" update Status is True ");
        qDebug () << " Start Updator , extractor ";
        qDebug () << " Closing This Application  ";
        // correct the command line parameter
        QFile appInfoFile(this->appInfoFilename);
        appInfoFile.remove();

        //QString applicationPath = QCoreApplication::applicationFilePath();
        QString applicationPath = "file:///" + QCoreApplication::applicationDirPath();
        applicationPath += "/iuredon.exe";

        fileDebug( " App Path = " + applicationPath );

        QDesktopServices::openUrl( QUrl( applicationPath ) );

        fileDebug( " Exiting  = 0 ");


        exit(0);
       // QDesktopServices::openUrl(QUrl("file:///<path to your exe>"), QUrl::TolerantMode));
    } else {
        fileDebug (" update Status is False ");
    }

}
