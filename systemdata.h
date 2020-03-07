#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include <QtCore>
#include <QDebug>
#include "downloadlist.h"
#include "lib.h"
#include "analytics.h"

class SystemData
{


public:
    /* ===============================================
     *      GLOBAL VARIABLES
    ==================================================*/
    QString homePath;
    QString defaultDownloadPath;
    QString tmpRootDir;
    QHash<QString, QString> mimeType;
    Analytics *redonUsageData;




    /*================================================
     *      GLOBAL METHODS
    ==================================================*/
    SystemData();
    int checkDefaultDownloadPath();
    int checkDownloadDirs( DownloadList *dList);
    int checkTmpDir();
    int loadMimeData(QString filename);
    QString getExtension(QString contentType );

    bool errorFlag;


};

#endif // SYSTEMDATA_H
