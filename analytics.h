#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QObject>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


struct usageData {

    int totalDownloadCount;
    int pauseResumeCount;
    int fileRetiresCount;
    int downloadCancelledCount;
};

class Analytics : public QObject
{
    Q_OBJECT
public:
    explicit Analytics(QObject *parent = 0);

    QFile *file;
    QDataStream *ds;

    void formatFile( QString filepath );
    void debug ();

    QString currentFilename;
    QString filepath;

    int incDLCount (); // Download Count
    int incPRCount(); // pause Resume Count
    int incFRCount(); // File Reties Count
    int incDCCount (); // Download Cancelled Count



    QByteArray getLogsJson();
    QJsonObject readLogFile (QString filename);

signals:

public slots:

};



#endif // ANALYTICS_H
