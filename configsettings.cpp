#include "configsettings.h"

ConfigSettings::ConfigSettings()
{
    this->maxConnections = 6;
}

void ConfigSettings::init()
{
    this->loadSettings();
}

void ConfigSettings::loadSettings()
{
    QFile xmlFile("settings.xml");
    if( !xmlFile.open(QIODevice::ReadOnly)) {
        qDebug () << " Cannot Open Settings File ";
        return;
    }

    QDomDocument document;
    document.setContent( &xmlFile );

    QDomElement root = document.firstChildElement();

    if( !root.isNull() ) {

        this->setConnection( root.elementsByTagName("max_connection").at(0).toElement().attribute("value").toInt() );

        this->SDocument = root.elementsByTagName("document").at(0).toElement().text();
        this->SCompressed = root.elementsByTagName("compressed").at(0).toElement().text();
        this->SMusic = root.elementsByTagName("music").at(0).toElement().text();
        this->SVideo = root.elementsByTagName("video").at(0).toElement().text();
        this->SApplication = root.elementsByTagName("application").at(0).toElement().text();
        this->SPicture = root.elementsByTagName("picture").at(0).toElement().text();



        this->parse( &this->HDocument , this->SDocument );
        this->parse( &this->HCompressed, this->SCompressed );
        this->parse( &this->HMusic , this->SMusic );
        this->parse( &this->HVideo , this->SVideo );
        this->parse( &this->HApplication , this->SApplication );
        this->parse( &this->HPicture , this->SPicture );


    } else {
        qDebug () << " Settings File si null ";
    }

}

void ConfigSettings::saveSettings()
{


    QFile xmlFile("settings.xml");
    if( !xmlFile.open(QIODevice::WriteOnly)) {
        return;
    }

    QDomDocument document;
    QDomElement root = document.createElement("root");

    document.appendChild( root );
    QDomElement maxConnectionNode = document.createElement("max_connection");
    maxConnectionNode.setAttribute("value", this->maxConnections);

    // file Type
    QDomElement fileType = document.createElement("file_type");

    QDomElement DocumentElement    = this->getDomElement("document", this->SDocument );
    fileType.appendChild( DocumentElement);

    QDomElement CompressedElement   = this->getDomElement("compressed", this->SCompressed );
    fileType.appendChild( CompressedElement );

    QDomElement MusicElement        = this->getDomElement("music", this->SMusic );
    fileType.appendChild( MusicElement );

    QDomElement VideoElement        = this->getDomElement("video", this->SVideo );
    fileType.appendChild( VideoElement );

    QDomElement AppElement          = this->getDomElement("application", this->SApplication );
    fileType.appendChild( AppElement );

    QDomElement PictureElement      = this->getDomElement("picture", this->SPicture );
    fileType.appendChild( PictureElement );



    root.appendChild( maxConnectionNode );
    root.appendChild( fileType );
    QTextStream stream( &xmlFile );
    stream << document.toString();
    xmlFile.close();


}

void ConfigSettings::setConnection(int i)
{
    this->maxConnections = i;

}

void ConfigSettings::setFileTypes(QString SDocument, QString SCompressed, QString SMusic, QString SVideo, QString SPicture, QString SApplication)
{
    this->SDocument = SDocument;
    this->SCompressed = SCompressed;
    this->SMusic = SMusic;
    this->SVideo = SVideo;
    this->SPicture = SPicture;
    this->SApplication = SApplication;

}

QDomElement ConfigSettings::getDomElement(QString name, QString value)
{
    QDomDocument document;
    QDomElement elem = document.createElement(name);
    //elem.setNodeValue( value );
    elem.appendChild( document.createTextNode( value));
    qDebug () << name << " = " <<  value ;
    return elem;

}

void ConfigSettings::parse(QSet<QString> *set, QString data)
{
    QStringList list = data.split(",");

    int len = list.length();
    for( int i = 0 ; i < len; i ++ ) {
        QString value = list[i].trimmed();
        set->insert( value );
    }

}



