#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QDebug>
#include <QMessageBox>


DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);
    this->downloadCompletedFlag = false;
    this->downloadedSize = 0;
    this->totalSize = 0;
    this->maxConnection = 8;
    this->seginfoIndex = 0;
    this->downloadedBytes = 0;
    this->lastDownloadedBytes = 0;

    this->canPlayFlag = false;

    this->dList = NULL;

    setWindowFlags( Qt::Dialog   | Qt::WindowMinimizeButtonHint    | Qt::WindowCloseButtonHint    | Qt::CustomizeWindowHint );

}

DownloadDialog::~DownloadDialog()
{
    qDebug () << " close download   ";
    delete ui;
}

void DownloadDialog::init(DownloadListItem *dListItem, DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings)
{

    this->dListItem = dListItem;
    this->dList = dList;
    this->sysData = sysData;
    this->cSettings = cSettings;
    this->totalSize = dListItem->filesize;
    this->maxBlocks = 0;
    this->previousDownloadedBytes = 0;


    this->stabilizerIndex = 0;
    this->stabilizerMax = 0;
    this->stabilizerTick = 0; // frquency divider
    this->lastTimeMsec = 0;

    for( int i = 0 ; i < MAX_STABILIZER; i++) {
        this->stabilizerArray[i] = 0;
    }



    this->maxConnection = this->cSettings->maxConnections;
    this->tmpDir = this->sysData->tmpRootDir;

    this->setupDialog();
   // this->drawTable();


    if( !QDir(this->tmpDir).exists()) {
        QMessageBox::warning(this, " Cannot Find Temp File ", " Cannot Find Temp Directory Please Create Directoy  " + this->tmpDir);
        return ;
    }

    if( this->dListItem->multipart == true ) {
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        this->tmpDir = this->tmpDir + "/" + QString::number(timestamp) + "/";

        if( QDir().mkdir(this->tmpDir) ) {
            this->dListItem->segmentDir = this->tmpDir;
            this->dstate = new DownloadState( this->tmpDir + "state.log");

        } else {
            QMessageBox::warning(this, " Cannot Find Temprory Directory  ", " Cannot Create Temprory Directory   " );
            return ;
        }

    }

    this->dListItem->setDownloadStatus( DownloadList::DOWNLOADING );
    this->dList->addItem( this->dListItem );

    this->start();

    this->show();


}

void DownloadDialog::resume(DownloadListItem *dListItem, DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings)
{

    this->dListItem = dListItem;
    this->dList = dList;
    this->sysData = sysData;
    this->cSettings = cSettings;
    this->totalSize = dListItem->filesize;
    this->maxBlocks = 0;
    this->previousDownloadedBytes = 0;

    this->stabilizerIndex = 0;
    this->stabilizerMax = 0;
    this->stabilizerTick = 0; // frquency divider
    this->lastTimeMsec = 0;


    this->maxConnection = this->cSettings->maxConnections;

    this->tmpDir = this->sysData->tmpRootDir;

    this->setupDialog();


    this->tmpDir = dListItem->segmentDir;
   // qDebug () << " resume 4"  << this->tmpDir + "state.log";
   // this->dstate = new DownloadState( this->tmpDir + "state.log");


    this->start();
    this->dListItem->setDownloadStatus(  DownloadList::DOWNLOADING );

    //  this->dList->addItem( this->dListItem );

    this->show();

}

void  DownloadDialog::getSegmentInfo()
{

    QString stateFile = this->dListItem->segmentDir + "state.log";
    QFile sFile( stateFile );

    if( sFile.exists()) {
      //  qDebug () <<"  Reading Segment ";
        this->readSegmentInfo();
    } else {
      //  qDebug () << " New Segment ";
         this->newSegmentInfo();
         this->dstate->open();
         this->dstate->init( this->segmentInfoArr, this->seginfoIndex );
    }


}

void DownloadDialog::newSegmentInfo()
{

    qint64 minSegSize = 1024 * 200;
    qint64 filesize = this->dListItem->filesize;
    qint64 segmentSize = filesize / this->maxConnection;

    while ( this->maxConnection > 1 && segmentSize < minSegSize ) {
        this->maxConnection -- ;
        segmentSize = filesize / this->maxConnection;
    }

    qint64 rangeStart = 0;
    qint64 rangeEnd = segmentSize;

    for( int i = 0 ; i < this->maxConnection;  i ++ ) {

        if( i == this->maxConnection -1) {
            rangeEnd = filesize;
        }

        this->segmentInfoArr[i].initial = rangeStart;
        this->segmentInfoArr[i].current = rangeStart;
        this->segmentInfoArr[i].end = rangeEnd;

        rangeStart = rangeEnd + 1;
        rangeEnd += segmentSize;

        this->seginfoIndex ++;

    }


}

void DownloadDialog::readSegmentInfo()
{

    this->dstate->readSegment( this->segmentInfoArr, &this->maxConnection);
    for( int i = 0 ; i < this->maxConnection; i ++) {
     //   qDebug () << "i = " << i
            //      <<"initial = " << this->segmentInfoArr[i].initial
           //       <<"current = " <<  this->segmentInfoArr[i].current
           //       <<"end = " <<  this->segmentInfoArr[i].end ;

    }
    this->seginfoIndex = this->maxConnection;

}

void DownloadDialog::setupDialog()
{

    this->setWindowTitle( this->dListItem->filename );
    QString lblFilesize = "unknown";

    if( this->dListItem->filesize > 0 ) {
        lblFilesize = sizeToString(  this->dListItem->filesize ) ;
    }

    this->ui->lbl_url->setText( this->dListItem->currentUrl );
    this->ui->lbl_filesize->setText( lblFilesize );


    this->ui->lbl_downloaded->setText(sizeToString(0));
    this->ui->lbl_transferRate->setText( sizeToString(0) + "/s"   );
    this->ui->lbl_timeLeft->setText( "unknown");

    // table model view

    if( this->dListItem->active) {
        this->dListItem->tModel->savedPath->setText( this->dListItem->getAbsoluteFilePath(false) );
        this->dListItem->tModel->size->setText( lblFilesize );
    }

    if( dListItem->multipart == true ) {
        this->ui->lbl_resumeCapability->setText( "Yes");
    } else {
        this->ui->lbl_resumeCapability->setText( "No");
    }



    if( this->dListItem->extension != "flv" ) {
        if( !this->dListItem->mimeType.contains("video") ) {
            this->ui->btn_play->setVisible( false );
            this->ui->horizontalSlider->setVisible( false );
            this->ui->play_fast_lbl->setVisible( false );
            this->ui->download_fast_lbl->setVisible( false );
        }
    }



}

void DownloadDialog::drawTable()
{

    /*
    QTableWidget *table = this->ui->tableWidget;
    QStringList stringList;

    table->setRowCount( this->cSettings->maxConnections );

    qDebug () << "config setting max settings  " << this->cSettings->maxConnections;
    table->setColumnCount( 5 );

    stringList << " N.. " << " Total Size " << " Downloaded " << " Transfer Rate " << " Progress ";

    table->setHorizontalHeaderLabels( stringList );
    table->verticalHeader()->setVisible( false );
    table->setShowGrid( false );

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setShowGrid(false);

    table->horizontalHeader()->resizeSection(0, 50);
    table->horizontalHeader()->resizeSection(1, 100);
    table->horizontalHeader()->resizeSection(2, 100);
    table->horizontalHeader()->resizeSection(3, 100);
    table->horizontalHeader()->resizeSection(4, 300);
    table->verticalHeader()->resizeSection(0, 30);
    */



}

void DownloadDialog::setupCubes()
{

    QPixmap whiteCube(":/cubes/cubes/white_cube.png");
    this->scene = new QGraphicsScene ();
    this->ui->graphicsView->setScene( this->scene );

    int x = 0 ;
    int y = 0;
    for( int i = 0 ; i < this->maxBlocks; i ++ ) {

        x = (i % 50) * 11;
        y = (i / 50) * 11;

        this->cubeItem[i] = scene->addPixmap( whiteCube );
        this->cubeItem[i]->setPos( x , y  );
    }

    for( int i = 0 ; i < this->bms->completedBlockList.size(); i ++  ) {
        this->updateCube( this->bms->completedBlockList[i], 2);
    }

}

void DownloadDialog::drawCubes()
{
 //   qDebug () << " draw Cubes "    ;
}



void DownloadDialog::updateCube(int index, int color)
{
    QPixmap whiteCube(":/cubes/cubes/white_cube.png");
    QPixmap blueCube(":/cubes/cubes/blue_cube.png");
    QPixmap yellowCube(":/cubes/cubes/yellow_cube.png");

    index -- ;
    index = ( index / this->cubeDivider );

    if( color == 0 ) {
        this->cubeItem[index]->setPixmap( whiteCube );
    } else if( color == 1) {
        this->cubeItem[index]->setPixmap( yellowCube );
    } else if( color == 2 ) {
        this->cubeItem[index]->setPixmap( blueCube );
    }

}

void DownloadDialog::start()
{
    this->stoppedFlag = false;
    this->stabilizerIndex = 0;

    if( this->dListItem->multipart == true ) {
        this->multipartDownload();
    } else {
        this->simpleDownload();
    }
    this->sysData->redonUsageData->incDLCount();
}

void DownloadDialog::stop()
{
    qDebug () << "\n\n\n ===================================  \n STOPPED ===============================\n\n";


    if( this->dListItem->multipart ) {
        disconnect( this->timer, SIGNAL(timeout()), this, SLOT (multipartUpdateStatus()));
        this->wp->stop();
    }
    this->timer->stop();
    this->stoppedFlag = true;
    this->dListItem->setDownloadStatus( DownloadList::STOPPED );
    ////this->dListItem->downloadStatus = DownloadList::STOPPED;
    emit downloadStopped();

}

void DownloadDialog::multipartDownload()
{

    qint64 filesize = this->dListItem->filesize;

    this->bms = new BlockManagementSystem( this->dListItem , filesize );
    this->mbm = new MasterBufferManager( this->dListItem );
    this->timer = new QTimer(this);
    this->cubeDivider = bms->getDivider();
    int blockSize = bms->getBlockSize();

 //   qDebug () << " cube Diviedr = "  << this->cubeDivider;
    qint64 totalBlocks = filesize / blockSize;
 //   qDebug () << " TotalBlocks = " << totalBlocks << "  ====== \n";

    this->maxBlocks = ( totalBlocks ) / this->cubeDivider;

    if( filesize % blockSize > 0 ) {
        this->maxBlocks ++;
    }
//    qDebug () << " max Blocks " << this->maxBlocks;

    // here i have to check the downloaded size...
    this->previousDownloadedBytes = this->bms->completedBlockList.size() * blockSize;

    this->setupCubes();

    if( mbm->init() != -1) {

        this->previousDownloadedBytes = this->mbm->bytesDownloaded( blockSize );
    //    qDebug () << " previoid Download Bytes = " << this->previousDownloadedBytes;

        wp = new WorkersPool(this->dListItem, bms, mbm, this->cSettings->maxConnections );
        connect ( wp, SIGNAL(updateCube(int,int)), this, SLOT(updateCube(int, int)));
        connect ( wp, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
        connect ( wp, SIGNAL(errorOccured(int,QString)), this, SLOT(errorOccured(int, QString)));
        connect ( timer, SIGNAL(timeout()), this, SLOT(multipartUpdateStatus()) );

        timer->start(100);
        wp->startWorking();


        this->downloadTime = new QTime();
        this->downloadTime->start();

    } else {
     //   qDebug () << " \n\n\n ============= Cannot Init =============\n\n  ";
    }


   // qDebug () << " Download Size = " << this->downloadedSize;


}

void DownloadDialog::simpleDownload()
{

    // simple Download Class

    this->sd = new SimpleDownload();

    this->sd->init( this->dListItem );

    timer = new QTimer( this );

    connect ( timer, SIGNAL(timeout()), this, SLOT(simpleUpdateStatus()) );
    connect ( this->sd, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()) );
    connect ( this->sd, SIGNAL(errorOccured(int,QString)), this, SLOT ( errorOccured(int, QString)));

    timer->start(100);

    this->downloadTime = new QTime();
    this->downloadTime->start();

/*
    this->maxConnection = 1;

    this->ui->btn_pause->setVisible( false );

    //QTableWidget *table = this->ui->tableWidget;
    QTableWidget *table;

    qDebug () << " filesize " << this->dListItem->filesize;
    this->sRow[0] = new SegmentRow( this->dListItem->filesize );
    qDebug () << " Simple Download 2";

    table->setItem( 0, 0, new QTableWidgetItem ( QString("0")) );
    table->setItem(0, 1, this->sRow[0]->lblTotalSize);
    table->setItem(0, 2, this->sRow[0]->lblDownloadedSize);
    table->setItem(0, 3, this->sRow[0]->lblTransferRate);
    table->setCellWidget( 0, 4, this->sRow[0]->pb);

    segmentInfo si;
    si.initial = 0 ;
    si.current = 0;
    si.end = this->dListItem->filesize;

    qDebug () << " Simple Download 3";

    this->sRow[0]->fDownloader = new FileDownloader(this->dListItem, si, 0, this->dstate);

    // file downloader  (updateStatus) --> status row (updateStatus)
    connect ( this->sRow[0]->fDownloader, SIGNAL(updateStatus(qint64,qint64,int)),
            this->sRow[0], SLOT(updateStatus(qint64,qint64,int)) );


    // file downloader (finishedDownload)  --> status row (finshedDownload)
    connect( this->sRow[0]->fDownloader, SIGNAL(finished(qint64)),
            this->sRow[0], SLOT(finishedDownload(qint64)) );


    connect ( this->sRow[0], SIGNAL(downloadFinished()) ,
            this, SLOT(downloadFinished()) );

    qDebug () << " Simple Download 4";

    this->sRow[0]->fDownloader->start();
    this->timer = new QTimer(this);
    connect ( this->timer, SIGNAL(timeout()), this, SLOT ( simpleUpdateStatus()) );
    this->timer->start( 150 );
    qDebug () << " Simple Download 5";
    */

}

void DownloadDialog::addTransferRate(qint64 rcvd)
{

    qint64 currentElapse = this->downloadTime->elapsed();
    qint64 dataDiff = rcvd - this->lastDownloadedBytes;
    qint64 timeDiff = currentElapse - this->lastTimeMsec;


    this->downloadedSize = this->previousDownloadedBytes + rcvd;
    this->lastDownloadedBytes = rcvd;
    this->lastTimeMsec = currentElapse;


    int currentTransferRate = 0;
    if( timeDiff > 0  && dataDiff > 0 ) {
        currentTransferRate = ( dataDiff * 1000 )  / timeDiff;
    }

   // qDebug () << " current transfer Rate = " << currentTransferRate << " datadiff = " << dataDiff  << " timediff = " << timeDiff ;


    if( this->stabilizerIndex ==  MAX_STABILIZER ) {
        this->stabilizerIndex = 0 ;
    }

    if( currentTransferRate == 0 ) {

        this->stabilizerTick ++;
        if( this->stabilizerTick == 3 ) {
            this->stabilizerTick = 0 ;
            this->stabilizerArray[ this->stabilizerIndex++ ] = 0;
        }

    } else {
       this->stabilizerArray[ this->stabilizerIndex++ ] = currentTransferRate;
    }


    if( this->stabilizerMax < MAX_STABILIZER ) {
        this->stabilizerMax ++;
    }

    qint64 avgDownloadedBytes = 0;
    int i = 0;

    for( i  ; i < this->stabilizerMax; i ++) {
        avgDownloadedBytes += this->stabilizerArray[ i ];
    }


    this->transferRate = avgDownloadedBytes / i;


    /*
    qint64 elapsedTime =  this->downloadTime->elapsed(); // convert msec to sec
    if( elapsedTime > 0  &&  this->downloadedSize )  {
        this->transferRate = ( this->downloadedSize * 1000 ) / elapsedTime;
    }*/


    /*
    qint64 diff = rcvd - this->lastDownloadedBytes;

    this->downloadedSize = rcvd;
    this->lastDownloadedBytes = rcvd;


    qDebug () << " dl size = " << this->downloadedSize << " time elapsed " <<  this->downloadTime->elapsed();

    if( this->stabilizerIndex ==  MAX_STABILIZER ) {
        this->stabilizerIndex = 0 ;
    }

    if( diff == 0 ) {
       // this->stabilizerTick ++;
       // if( this->stabilizerTick == 5 ) {
       //     this->stabilizerTick = 0;
        //    this->stabilizerArray[ this->stabilizerIndex++ ] = 0;
       // }

    } else {
        this->stabilizerArray[ this->stabilizerIndex++ ] = diff;
    }


    if( this->stabilizerMax < MAX_STABILIZER ) {
        this->stabilizerMax ++;
    }

    qint64 avgDownloadedBytes = 0;
    int i = 0;

    for( i  ; i < this->stabilizerMax; i ++) {
        avgDownloadedBytes += this->stabilizerArray[ i ];
    }

    this->transferRate = avgDownloadedBytes / i;

    qint64 elapsedTime =  this->downloadTime->elapsed();
    if( elapsedTime > 0  && this->downloadedSize > 0 ) {
         this->transferRate = this->downloadedSize / ( elapsedTime / 1000 ) ;
    }

    qDebug () << " Diff xyz = " << sizeToString( this->transferRate ) <<" " <<  this->transferRate;
    */

}

void DownloadDialog::updateStatus()
{


/*
    float transferRate = this->wp->getTransferRate();
    float percentCompleted;
    if( transferRate > 0 ) {
        qDebug () << " percent Completed " << percentCompleted;
        QString str = sizeToString( transferRate );
        qDebug () << "transfer Rate = " << str;
        //this->ui->label->setText( str );
        //this->ui->label->setText( str );
        //qDebug () << "updating status " << str;
    }
*/


    float percentCompleted = 0;

    if( this->totalSize > 0  && ( this->downloadedSize <= this->totalSize ) ) {

        percentCompleted = (float) ( this->downloadedSize * 100 ) / (float) this->totalSize;

   //     qDebug () << " % " << percentCompleted << " downloaded Size = " << this->downloadedSize << "  total size " << this->totalSize;

        this->ui->progressBar->setValue( percentCompleted );
        this->dListItem->percentCompleted = percentCompleted;
        if( this->dListItem->active) {
            this->dListItem->tModel->status->setText( QString::number( percentCompleted, 'g', 2) + "%" );
        }

        if( percentCompleted > 2 ) {
            if( this->canPlayFlag == false ) {
                this->ui->btn_play->setEnabled( true );
            }
            this->canPlayFlag = true;

        }

        qint64 remainingData = (this->totalSize - this->downloadedSize );
        QString strTimeRemaining = "unknown";

        if( this->transferRate > 0 ) {

            int timeRemaining = ( remainingData / this->transferRate );
            strTimeRemaining = timeToString( timeRemaining );
            this->dListItem->timeLeft = timeRemaining;


        }
        this->ui->lbl_timeLeft->setText( strTimeRemaining );
        if( this->dListItem->active ) {
            this->dListItem->tModel->setTimeLeft( strTimeRemaining );
        }

      //  qDebug () << " transfer Rate = " << this->transferRate << " downloaded = " << sizeToString( this->downloadedSize);
    }

    QString strTransferRate  = sizeToString( this->transferRate );
    this->ui->lbl_transferRate->setText( strTransferRate + "/s");
    if( this->dListItem->active ) {
        this->dListItem->tModel->transferRate->setText( strTransferRate + "/s" );
    }

    this->ui->lbl_downloaded->setText( sizeToString( this->downloadedSize) + " ( " + QString::number(percentCompleted, 'f', 2 )+" % )" );
    this->dListItem->currentSpeed = this->transferRate;

    /*

    float percentCompleted;

    if ( this->totalSize > 0 ) {

        percentCompleted = (float)(this->downloadedSize * 100) / (float)this->totalSize;
        this->ui->progressBar->setValue( percentCompleted );

        if( this->transferRate > 0 ) {

            qint64 dataRemaining = (this->totalSize - this->downloadedSize);
            int timeRemaining = ( dataRemaining / this->transferRate );

            QString strTimeRemaining = timeToString( timeRemaining );
            this->ui->lbl_timeLeft->setText( strTimeRemaining );

            //this->tModel->timeLeft->setText( strTimeRemaining );
            if( this->dListItem->active) {
                this->dListItem->tModel->setTimeLeft( strTimeRemaining );
            }


            this->dListItem->currentSpeed = this->transferRate;
            this->dListItem->timeLeft = timeRemaining;

        }
        this->dListItem->percentCompleted = percentCompleted;
        if( this->dListItem->active) {
            this->dListItem->tModel->status->setText( QString::number( percentCompleted, 'g', 2) + "%" );
        }

    }

    QString strTransferRate = sizeToString( this->transferRate );
    this->ui->lbl_transferRate->setText( strTransferRate + "/s" );

    if( this->dListItem->active) {
       this->dListItem->tModel->transferRate->setText( strTransferRate + "/s" );
    }

    this->ui->lbl_downloaded->setText( sizeToString( this->downloadedSize) + " ( " + QString::number(percentCompleted, 'f', 2 )+" % )" );
    this->dListItem->currentSpeed = this->transferRate;
*/



}

void DownloadDialog::errorOccured(int errorCode, QString errorString)
{

    QString errorMessage = " Error Occured during download. \n url = " + this->dListItem->currentUrl + " \n\n" + errorString;

    if( this->dListItem->multipart ) {
        disconnect( this->timer, SIGNAL(timeout()), this, SLOT (multipartUpdateStatus()));
        this->wp->stop();
    }
    this->timer->stop();
    this->stoppedFlag = true;
    this->close();
    QMessageBox::warning(this, " Error Occured ", errorMessage );

}

QThread::Priority DownloadDialog::getThreadPriority(int i)
{

     return QThread::NormalPriority;
    /*

    switch ( i ) {
        case 1: return QThread::HighestPriority;
        case 2: return QThread::HighPriority;
    default :
        return QThread::NormalPriority;

    }*/
}


void DownloadDialog::simpleUpdateStatus()
{

    /*

   // qDebug() <<" us 1";
    QMutex mutex;
    mutex.lock();
    this->downloadedSize = this->sRow[0]->downloadedSize;

    if( this->sRow[0]->totalSize > 0 ) {
        this->totalSize = this->sRow[0]->totalSize;
    }
   // qDebug() <<" us 2" ;
    this->sRow[0]->updateProgress();
  //  qDebug() <<" us 3" ;
    this->transferRate = this->sRow[0]->transferRate;
  //    qDebug() <<" us 4" ;
    mutex.unlock();
       // qDebug() <<" us 5"<< this->totalSize ;
    QString filesize = "unknown";

    if( this->totalSize > 0) {
     //   qDebug() <<" us 6" ;
        filesize = sizeToString( this->totalSize ) ;
    }

    this->ui->lbl_filesize->setText( filesize );
    if( this->dListItem->active) {
        this->dListItem->tModel->size->setText( filesize );
    }

    this->updateStatus();

    */

    QString strFileSize = "unknown";


    this->totalSize = sd->total;
    if( this->totalSize > 0 ) {
        strFileSize = sizeToString( this->totalSize );
    }

    this->ui->lbl_filesize->setText( strFileSize );
    this->addTransferRate( sd->rcvd );
    this->updateStatus();

}

void DownloadDialog::downloadFinished()
{

  //  qDebug () << "  DOWNLOAD FINISHED 11  ";

    disconnect ( timer, SIGNAL(timeout()), this, SLOT(multipartUpdateStatus()) );
    this->timer->stop();

    this->downloadCompletedFlag = true;
    this->ui->progressBar->setValue( 100 );

    if( this->dListItem->multipart ) {
        //this->multipartUpdateStatus();
    } else {
        this->simpleUpdateStatus();
    }

    this->dListItem->filesize = this->totalSize;
    emit downloadCompleted();
    this->close();


}

void DownloadDialog::closeEvent(QCloseEvent *event)
{
    this->stop();
    event->accept();
}


void DownloadDialog::multipartUpdateStatus()
{
    this->addTransferRate( this->wp->getBytesDownloaded());
    this->updateStatus();
}

void DownloadDialog::on_btn_pause_clicked()
{
    this->stop();
    this->ui->btn_resume->setEnabled( true );
    this->ui->btn_pause->setEnabled( false );
}

void DownloadDialog::on_btn_resume_clicked()
{
    qDebug () << "\n\n\n==================== RESUMING ==================== \n\n\n ";
    this->start();
    this->ui->btn_resume->setEnabled( false );
    this->ui->btn_pause->setEnabled( true );
    this->on_horizontalSlider_valueChanged(0);
    this->sysData->redonUsageData->incFRCount();

}



void DownloadDialog::on_btn_play_clicked()
{
    QString filePath =  this->dListItem->getAbsoluteFilePath( false );
    QDesktopServices::openUrl( "file:///" +  filePath);
}

void DownloadDialog::on_horizontalSlider_valueChanged(int value)
{
    int sliderValue =  this->ui->horizontalSlider->value();
    int diff = 50 - sliderValue;
    this->wp->setTickValue(diff);
}
