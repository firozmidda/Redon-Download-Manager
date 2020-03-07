#include "workerspool.h"

WorkersPool::WorkersPool(QObject *parent) :
    QObject(parent)
{
}

WorkersPool::WorkersPool(DownloadListItem *dListItem, BlockManagementSystem *bms, MasterBufferManager *mbm, int maxConnection)
{
    this->dListItem = dListItem;
    this->bms = bms;
    this->mbm  = mbm;
    this->maxWorkers = maxConnection;
    this->currentMaxWorker = 1;

    this->bytesDownloaded = 0;
    this->totalWorkers = 0;

    this->tick = 0;
    this->errorCount = 0;

    this->maxTicks = MAX_TICK;
    this->workFinishedFlag = false;
    this->downloadCompleted = false;
    this->stoppedFlag = false;


 //   qDebug () << " MAX TICK " << MAX_TICK;
 //   qDebug () << " TIMESTAMP DURATION " << TIMESTAMP_DURATION;

}


void WorkersPool::startWorking()
{


    if( this->pendingBlocks.size() > 0 ) {
        BlockInfo tmpBlock = this->pendingBlocks[0];

        int index = this->bms->getCurrentIndex();
        if( index - tmpBlock.index >= currentMaxWorker  ) {

            qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
            int diff =  ( timestamp  - tmpBlock.timestamp) / 1000;

            if( diff > (TIMESTAMP_DURATION )) {

                tmpBlock.timestamp = timestamp;
                this->removePendingList( tmpBlock );
                this->stopPendingBlock( tmpBlock );

                this->createWork( tmpBlock );
               // qDebug () << " From Pending List " << tmpBlock.index;
                return ;
            }
        }
    }


    if( this->workerList.size() == 0  && workFinishedFlag == true ) {
        this->mbm->finish();
        this->downloadCompleted = true;
        emit downloadFinished();

    } else  if( this->workerList.size() < this->currentMaxWorker &&  workFinishedFlag == false ) {

        BlockInfo bi = this->bms->getBlock();

        if( bi.end == 0 ) {
            this->workFinishedFlag = true;
        } else {
            this->createWork( bi );
        }

    }

}

void WorkersPool::createWork(BlockInfo bi)
{

  //  qDebug () << " CreateWork " << bi.index;

    this->pendingBlocks.append( bi );

    qint64 size = bi.end - bi.start;

    Worker *w = new Worker(this->dListItem, bi, this->mbm);
    connect (w, SIGNAL(updateProgress(qint64)), this, SLOT(updateTransferRate(qint64)));
    connect (w, SIGNAL(updateCube(int, int)), this, SIGNAL(updateCube(int,int)) );
    connect (w, SIGNAL(workFinished(BlockInfo, int , QString, Worker *)), this, SLOT(finishedWork( BlockInfo, int, QString, Worker *)));

    this->workerList.append( w );

    w->startWroking();
    totalWorkers ++;

    emit updateCube( bi.index, 1);

}

void WorkersPool::debug()
{
    qDebug () << " ================ DEBUG ============  ";
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    qDebug () << " Total Workers = " << this->totalWorkers << " Total Pending list = " << this->pendingBlocks.size();

    int len = this->pendingBlocks.size();
    for( int i = 0 ; i < len; i ++ ) {
        BlockInfo bi = this->pendingBlocks[i];
        qDebug () << " index = " <<  bi.index << " start  = " << bi.start
                  << " timestamp diff " <<  (timestamp - bi.timestamp) / 1000  << "/sec" ;
    }

    qDebug () << " Worker List  = ";
    for( int i = 0 ; i < this->workerList.size(); i ++ ) {
        qDebug () << this->workerList[i]->bi.index;
    }

    qDebug () << " ================ END ============  \n\n\n";

}

void WorkersPool::stop()
{

    qDebug () << "  \n\n ================== WORKER POOL  STOP () ============== \n\n ";

    this->stoppedFlag = true;
    for( int i = 0 ; i < this->workerList.size(); i ++ ) {
        this->workerList[i]->stop();
    }   
    this->mbm->stop();
}

void WorkersPool::removePendingList(BlockInfo bi)
{
    int len = this->pendingBlocks.size();
    BlockInfo tmpInfo;

    for( int i = 0 ; i < len;  i ++ ) {
        tmpInfo = this->pendingBlocks[i];

        if( tmpInfo.index == bi.index ) {
            this->pendingBlocks.removeAt(i);
            break;
        }
    }

}

void WorkersPool::removeFinishedIndex(int index)
{

    for( int i = 0 ; i < this->workerList.size(); i ++ ) {
        if( this->workerList[i]->bi.index ==  index ) {

            this->workerList.removeAt(i);
            return;
        }
    }
}

void WorkersPool::stopPendingBlock(BlockInfo bi)
{
    for( int i = 0 ; i < this->workerList.size(); i++ ) {

        if( this->workerList[i]->bi.index == bi.index ) {
            this->workerList[i]->stop();
            this->workerList.removeAt( i );
        }
    }


}

void WorkersPool::closeThread(BlockInfo bi)
{
    /*
    for( int i = 0 ; i < this->totalWorkers; i ++) {
        if( this->workerArray[i]->bi.index == bi.index ) {
            this->workerArray[i]->thread()->exit();
        }
    }*/
}

void WorkersPool::setTickValue(int value)
{

    QMutex mutex;
    mutex.lock();
    this->maxTicks = (MAX_TICK + (value * 2 ) );
    if( this->maxTicks <= 0) {
        this->maxTicks = 1;
    }
    mutex.unlock();
  //  qDebug () << " maxtick  = " << this->maxTicks;

}

void WorkersPool::addTransferRate(int rate)
{
}

qint64 WorkersPool::getBytesDownloaded()
{
    /**
        1. if total worker is 0  then start immidietly
        2. else divide the tick and start on max tick

    */
    if( !this->downloadCompleted ) {

        if( this->workerList.size() <= 0) {
            this->tick = 0;
            this->startWorking();
        } else {
            if( this->tick ==  this->maxTicks ) {
                this->tick = 0;
                this->startWorking();
            } else {
                this->tick ++;
            }
        }
    } else {
        bytesDownloaded = 0;
    }

    return bytesDownloaded;
}

bool lessThan ( const BlockInfo &bi1, const BlockInfo &bi2 ) {
    return bi1.index < bi2.index;
}

void WorkersPool::finishedWork(BlockInfo bi, int errorCode, QString errorString, Worker *w)
{

    this->currentMaxWorker = ( this->mbm->mergeIndex / 3) + 1 ;
    if( this->currentMaxWorker > this->maxWorkers ) {
        this->currentMaxWorker = this->maxWorkers;
    }

    this->currentMaxWorker  = 8;
    if( this->stoppedFlag == false ) {

        if( errorCode == 0 ) {
            this->removePendingList (bi );
            qSort( this->pendingBlocks.begin(), this->pendingBlocks.end(), lessThan  );
        } else {

            if( this->errorCount > 3) {
                emit errorOccured(  errorCode, errorString );
            }

            this->errorCount ++;
           // qDebug () << " ErrorCount = " << this->errorCount;
            Sleep( 500 );
        }

        this->removeFinishedIndex( bi.index );
        this->totalWorkers -- ;


        if( this->totalWorkers == 0 ) {
          //  qDebug () << " FinishedWork: Total Worker = 0 " ;
        } else {
           // qDebug () << " FinishedWork: Total Worker Not  0 " ;
            if( this->totalWorkers < (this->maxWorkers -1 ) ) {
                this->startWorking();
            }
        }

    }

    this->deleteWorker.append( w );

   // qDebug () << " Added To Delete Worker " << this->deleteWorker.size();
    if( this->deleteWorker.size() > 20 ) {

        Worker *tmpW = this->deleteWorker.takeFirst();
//        qDebug () << " Deleting Worker Index = " << tmpW->bi.index << this->deleteWorker.size();
        delete tmpW;
    }

}

void WorkersPool::updateTransferRate(qint64 rcvd)
{    
    this->bytesDownloaded += rcvd;

/*
    if( this->vacantSize < 0) {
        qDebug () << " Vacant Size = " << vacantSize;

      ///  int halfReceived = rcvd / 2;
      ///  this->vacantSize += halfReceived;
        this->vacantSize += rcvd;

        qDebug () << " Vacant Size2 = " << vacantSize;
        if( this->vacantSize >= 0 ) {
            this->bytesDownloaded += this->vacantSize;
        }
        //this->bytesDownloaded += (rcvd - halfReceived );

    } else {
        this->bytesDownloaded += rcvd;
    }
*/

}

