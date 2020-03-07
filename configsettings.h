#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include <QtCore>
#include <qDebug>
#include <QtXml>

class ConfigSettings
{
public:
    ConfigSettings();
    void init();
    void loadSettings();
    void saveSettings();
    void setConnection(int i);
    void setFileTypes ( QString SDocument,QString SCompressed,QString SMusic,QString SVideo,QString SPicture,QString SApplication );
    QDomElement getDomElement( QString name, QString value );

    void parse(QSet<QString> *set, QString data);

    /* =================================
     *   CONFIGURATION VARIABLES
      ===================================*/


     int maxConnections;
     QString SDocument, SCompressed, SMusic, SVideo, SPicture, SApplication;
     QSet<QString> HDocument, HCompressed, HMusic, HVideo, HPicture, HApplication;

};

#endif // CONFIGSETTINGS_H
