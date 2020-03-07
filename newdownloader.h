#ifndef NEWDOWNLOADER_H
#define NEWDOWNLOADER_H

#include <QObject>
#include <QStandardItemModel>
#include "downloadlist.h"
#include "systemdata.h"
#include "confirmdialog.h"
#include "downloaddialog.h"
#include "configsettings.h"
#include "finisheddialog.h"
#include "helperclasses.h"


class NewDownloader : public QObject
{
    Q_OBJECT
public:
    explicit NewDownloader(QObject *parent = 0);
    void init(QString url, DownloadList *dList, SystemData *sysData , ConfigSettings *cSettings);
    void resumeDownload(DownloadListItem *dListItem, DownloadList *dList,  SystemData *sysData , ConfigSettings *cSettings );

signals:
    void createModelRow( TableModel *tModel);
    void refereshDownloadList();
    
public slots:
    void downloadConfirmed ( DownloadListItem *dListItem);
    void downloadCompleted();
    void downloadStopped();
    
private:
    QString url;

    DownloadList *dList;
    SystemData *sysData;
    ConfirmDialog *cDialog;
    DownloadDialog *dDialog;
    FinishedDialog *fDialog;
    ConfigSettings *cSettings;
    TableModel *tModel;

    DownloadListItem *dListItem;

};

#endif // NEWDOWNLOADER_H
