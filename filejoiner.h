#ifndef FILEJOINER_H
#define FILEJOINER_H

#include <QObject>
#include <QtCore>
#include "downloadlist.h"

class FileJoiner : public QObject
{
    Q_OBJECT
public:
    explicit FileJoiner(QObject *parent = 0);
    // FileJoiner (DownloadInfo dInfo, QString tmpDir, int connection);
    FileJoiner ( DownloadListItem *dListItem, int maxConnection );
    void joinFile();
    
signals:
    
public slots:
    
private:
    DownloadListItem *dListItem;
    int maxConnection;
    QFile *file;

};

#endif // FILEJOINER_H
