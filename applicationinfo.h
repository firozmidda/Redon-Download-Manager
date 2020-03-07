#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QtCore>
#include <QtXml>
#include "defs.h"
#include "lib.h"
#include <QDesktopServices>
#include <QUrl>


class ApplicationInfo
{
public:
    ApplicationInfo();
    void checkAndCreateAppInfo();
    void readAppInfo();
    void processAppInfo();


private:
    bool updateStatusFlag;
    QString updateFilePath;
    QString appInfoFilename;

};

#endif // APPLICATIONINFO_H
