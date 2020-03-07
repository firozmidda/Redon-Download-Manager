#include "newdownloader.h"
#include <QDebug>

NewDownloader::NewDownloader(QObject *parent) :
    QObject(parent)
{

}

void NewDownloader::init(QString url, DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings)
{

    this->url = url;
    this->dList = dList;
    this->sysData = sysData;
    this->cSettings = cSettings;

    this->cDialog = new ConfirmDialog();
    connect ( this->cDialog, SIGNAL(downloadConfirmed(DownloadListItem*)), this, SLOT(downloadConfirmed(DownloadListItem*)));
    this->cDialog->init ( this->url, this->dList, this->sysData , this->cSettings);

}

void NewDownloader::resumeDownload(DownloadListItem *dListItem,  DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings)
{
    this->dListItem = dListItem;
    this->dList = dList;
    this->cSettings = cSettings;
    this->sysData = sysData;


    this->dDialog = new DownloadDialog();

    connect( this->dDialog, SIGNAL(downloadCompleted()), this, SLOT(downloadCompleted()) );
    connect ( this->dDialog, SIGNAL(downloadStopped()) , this, SLOT(downloadStopped()) );


    this->dDialog->resume( dListItem, this->dList, this->sysData, this->cSettings);


    /*
    this->dListItem = dListItem;
    this->dDialog = new DownloadDialog();
    this->tModel = new TableModel ( dListItem->currentUrl, dListItem->filename );

    connect( this->dDialog, SIGNAL(downloadCompleted()), this, SLOT(downloadCompleted()) );
    connect ( this->dDialog, SIGNAL(downloadStopped()) , this, SLOT(downloadStopped()) );
    this->dDialog->init(dListItem, this->dList, this->sysData, this->cSettings, this->tModel);

    emit createModelRow( tModel );
*/

}

void NewDownloader::downloadConfirmed(DownloadListItem *dListItem)
{

    this->dListItem = dListItem;
    this->dDialog = new DownloadDialog();
    this->dListItem->tModel = new TableModel ( dListItem->currentUrl, dListItem->filename , QIcon());

    connect( this->dDialog, SIGNAL(downloadCompleted()), this, SLOT(downloadCompleted()) );
    connect ( this->dDialog, SIGNAL(downloadStopped()) , this, SLOT(downloadStopped()) );
    this->dDialog->init(dListItem, this->dList, this->sysData, this->cSettings);
    emit createModelRow(  this->dListItem->tModel );

}

void NewDownloader::downloadCompleted()
{
     this->fDialog = new  FinishedDialog();
     this->dListItem->setDownloadStatus( DownloadList::COMPLETED );

    if( this->dListItem->active) {

        if( this->dListItem->tModel ) {
            this->dListItem->tModel->status->setText( this->dList->statusString[DownloadList::COMPLETED] );
        } else {
          this->tModel->status->setText(this->dList->statusString[DownloadList::COMPLETED] );
        }

    }

    this->dList->saveList();
    this->fDialog->init( this->dListItem );



    if( this->dListItem->segmentDir != "") {

        qDebug () << "\n\n\n ===============================  ATTENTION HERE ============================ ";
        qDebug () << "\n\n\n  Segment Directory  = "  << this->dListItem->segmentDir << "\n\n\n";

        QDir qdir( this->dListItem->segmentDir );
        qdir.removeRecursively();
    }

}

void NewDownloader::downloadStopped()
{ 
    emit refereshDownloadList();    
}
