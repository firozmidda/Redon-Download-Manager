#include "segmentrow.h"
#include <QDebug>

SegmentRow::SegmentRow(QObject *parent) :
    QObject(parent)
{
}

SegmentRow::SegmentRow(qint64 len)
{

    this->totalSize = len;
    this->downloadedSize = 0;
    this->newSize = 0;
    this->transferRate = 0 ;
    this->transferIndex = 0 ;
    this->finishedFlag = false;
    this->dTime = new QTime();
    this->dTime->start();
    this->timeStart = this->dTime->elapsed();
    this->transferAverageMax = 1;

    for( int i = 0 ; i < TRANSFER_ARR_SIZE; i ++ ) {
        this->transferRateArr[i] = 0;
    }


    this->pb = new QProgressBar();
    this->pb->setStyleSheet( " QProgressBar {  border: 1px solid #bcbcbc; background: #e6e6e6;  text-align: center;  color: #333; margin: 3px; } QProgressBar::chunk {   background-color: #06b025; width: 1px;  } " );
    this->lblTotalSize = new QTableWidgetItem();
    this->lblDownloadedSize = new QTableWidgetItem();
    this->lblTransferRate = new QTableWidgetItem();
    this->pb->setValue(0);


}

void SegmentRow::clear()
{

    delete this->dTime;
    //  delete progress bar later
    delete this->lblTotalSize;
    delete this->lblDownloadedSize;
    delete this->lblTransferRate;

 /*
    delete this->dTime;
    delete this->pb;
    delete this->lblTotalSize;
    delete this->lblDownloadedSize;
    delete this->lblTransferRate;
*/

}

void SegmentRow::updateProgress()
{

    int value = 0;
    if ( this->totalSize <= 0 ) {
        this->lblTotalSize->setText( "unknown" );
    } else if( this->totalSize > 0) {


        this->lblTotalSize->setText( sizeToString(this->totalSize) );
        value = (this->downloadedSize * 100) / this->totalSize;
        if( value > 49 && value < 55) {
              this->pb->setStyleSheet( " QProgressBar {  border: 1px solid #bcbcbc; background: #e6e6e6;  text-align: center;  color: white; margin: 3px; } QProgressBar::chunk {   background-color: #06b025; width: 1px;  } " );
        }
        this->pb->setValue(value);
    }


    this->lblDownloadedSize->setText( sizeToString(this->downloadedSize));
    this->lblTransferRate->setText( sizeToString( this->transferRate) + "/s" );


}

void SegmentRow::updateStatus(qint64 rcvd, qint64 total, int nCount)
{

   // if( nCount == 0 ) {
   //   qDebug () << "rcvd = "<< rcvd << " total = " << total ;
  //  }
   // qDebug () << "rcvd = "<< rcvd << " total = " << total ;
    this->downloadedSize = rcvd;
    this->totalSize = total;
    int byteReceived = rcvd - this->newSize;
    this->newSize = rcvd;


    this->timeEnd = this->dTime->elapsed();
    int diffTime = this->timeEnd - this->timeStart;
    if( diffTime > 0 ) {
        int speed = ( byteReceived  * 500) / diffTime;

        if( speed < 0 ) {
            speed = 0;
        }


        this->transferRateArr[ this->transferIndex] = speed;
        this->transferIndex = (this->transferIndex + 1) % TRANSFER_ARR_SIZE;

        qint64 totalTransfer = 0;
        for( int i = 0; i < this->transferIndex ; i ++) {
            totalTransfer += this->transferRateArr[i];
        }

        this->transferRate = (totalTransfer / this->transferAverageMax);

        this->timeStart = this->timeEnd;


        if( this->transferAverageMax <= TRANSFER_ARR_SIZE ) {
            this->transferAverageMax ++;
        }

    }



}

void SegmentRow::finishedDownload(qint64 totalSize)
{

    qDebug () << " Finish Start ";
    this->pb->setValue(100);
    this->totalSize = totalSize;
    this->finishedFlag = true;
    qDebug () << " Finish End ";

    emit downloadFinished();
}
