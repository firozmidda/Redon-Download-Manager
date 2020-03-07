#include "masterbuffermanager.h"

MasterBufferManager::MasterBufferManager(DownloadListItem *dListItem)
{

    this->dListItem = dListItem;

    logFilePath = this->dListItem->segmentDir + "state.log";
    mergeFilePath = this->dListItem->segmentDir + "merge.log";
    outputFilePath = this->dListItem->downloadRoot + this->dListItem->filename;

    this->logFile = new QFile( logFilePath );
    this->mergeFile = new QFile( mergeFilePath );
    this->outputFile = new QFile( outputFilePath);
    this->mergeIndex = 1;

}

int MasterBufferManager::init()
{

    this->readMergeLog();

    if( !this->logFile->open( QIODevice::Append ) ) {
        return -1;
    }

    if( !this->mergeFile->open( QIODevice::Append))  {
        return -1;
    }

    if( !this->outputFile->open( QIODevice ::Append)) {
        return -1;
    }
    qDebug () <<" Output File pos = "  << this->outputFile->pos();

}

int MasterBufferManager::stop()
{

    this->saveMergeLog();

    if( this->logFile->isOpen()) {
        this->logFile->flush();
        this->logFile->close();
    }

    if( this->mergeFile->isOpen() ) {
        this->mergeFile->flush();
        this->mergeFile->close();
    }

    if( this->outputFile->isOpen()) {
        this->outputFile->close();
    }

}


//http://r8---sn-h557sne7.googlevideo.com/videoplayback?key=yt5&signature=7E6C55EE788D2EB5C411A6E1B1EA5A05A85082C8.A47687E8CB12C37B706C0461FAAFCE1AA3080D9D&ratebypass=yes&source=youtube&gcr=in&id=o-ANsxFZJaw57lF15AI8grqn25u5ypxF2e_zCciaWs5nYE&expire=1394732386&ms=au&ip=117.97.143.1&itag=18&upn=FUwbm4ysDHI&sparams=gcr%2Cid%2Cip%2Cipbits%2Citag%2Cpcm2fr%2Cratebypass%2Csource%2Cupn%2Cexpire&fexp=900064%2C901454%2C937417%2C913434%2C936910%2C936913%2C902907%2C934022&pcm2fr=yes&sver=3&ipbits=0&mv=m&mt=1394708643&title=Take%20your%20startup%20to%20the%20next%20level

void MasterBufferManager::flushBuffer(int index)
{

    char stateLine[100];
    this->pendingBlocks.append( index );

    /**
    QString indexFilePath = this->dListItem->segmentDir + QString::number( index );

    QFile *tmpFile = new QFile( indexFilePath );

    if( tmpFile->open(QIODevice::WriteOnly ) ) {

        tmpFile->write( buffer, size );
        tmpFile->flush();
        tmpFile->close();


    } else {
        qDebug () << " flush Buffer " << " Cannot Open File ";
    }

    if( tmpFile ) {
        delete tmpFile;
    }
*/


    sprintf(stateLine, "%d\r\n", index);
    this->logFile->write( stateLine );

    // flush will be removed later
    this->logFile->flush();

}

bool MasterBufferManager::merge(int index)
{

    bool resultFlag = false;
    bool hasMerged = false;



    indexList.append( index );
    qSort( indexList.begin(), indexList.end() );

    qDebug () << " ============ Merge Start Here =========";
    int len = indexList.size();

    for( int i = 0; i < len;  i ++  ) {


        int tmpIndex = indexList[i];
        qDebug () << tmpIndex << " == " << this->mergeIndex;
        if( tmpIndex == this->mergeIndex ) {

           if( this->appendFile( tmpIndex ) != -1) {

               hasMerged = true;
               indexList.removeAt(i);
               len = indexList.size();
               this->mergeIndex ++;

               i -- ;

           }

        } else {

        }

    }
    qDebug () << "============= Merge Ends Here ==============";


    if( hasMerged ) {
        this->saveMergeLog();
    }

    return resultFlag;
}

void MasterBufferManager::finish()
{

    this->logFile->close();
    this->mergeFile->close();
    this->outputFile->close();

}

int MasterBufferManager::appendFile(int index)
{

    qDebug () << " Calling Append File index =  " << index ;
    QString indexFilePath = this->dListItem->segmentDir + QString::number( index );

    QFile *src = new QFile( indexFilePath );
    if( src->open( QIODevice::ReadOnly)) {


        qDebug () << " Write->output file " << outputFile->write( src->readAll() );
        src->close();

        qDebug () << " Remove Index File " << index << " status = " << src->remove();
        //qDebug () << " Remove Index File " << index << " status = " << QFile::remove( QString::number(index) );

        this->pendingBlocks.removeOne( index );
        qDebug () << " merge Index = " << index;


    } else {
        qDebug () << " cannot Open Src File ";
        return -1;
    }

    this->outputFile->flush();

    return 0;


}

void MasterBufferManager::readMergeLog()
{

    qDebug() << "\n\n\n ========================  Merge Start ================ \n\n\n ";


    QFile *tmpFile = new QFile( this->mergeFilePath );
    if( tmpFile->open( QIODevice::ReadOnly)) {
        QTextStream st( tmpFile );
        int index = 1;
        QStringList temp;
        while(!st.atEnd()) {
            // index =  st.readLine().toInt();
            QStringList arr = st.readLine().split(":");
            index = arr[0].toInt();
            if( arr.length() > 1 ) {
                temp = arr[1].split(",");
            }
        }


        if( temp.length() > 0 ) {
            for( int i = 0 ; i < temp.length(); i ++ ) {
                if( temp[i] != "") {
                    this->indexList.append( temp[i].toInt());
                }
            }
        }

        this->mergeIndex = index;
        qDebug () << " Index List = " << this->indexList;
        qDebug() << "\n\n\n ======================= MERGE ENDS ================================\n\n\n";
        tmpFile->close();
    }


/*
    if( this->mergeFile->isReadable() ) {

        QTextStream st( this->mergeFile );

        qint64 pos = st.pos();
        qDebug () << " file Position = " << pos;

        st.seek(0);

        int index = 1;
        while(!st.atEnd()) {
            index =  st.readLine().toInt();
        }

        this->mergeIndex = index;
        qDebug () << " \n\n\n =========  Merge Log Current Position ========= \n\n\n " << this->mergeIndex;
        st.seek( pos );

    }
*/

}

void MasterBufferManager::saveMergeLog()
{
    char mergeStr[100];
    QStringList list;

    for ( int i = 0 ; i < this->indexList.size(); i ++ ) {
        list << QString::number(this->indexList[i]);
    }

    QString line = list.join(",");

    QString result = QString::number(this->mergeIndex) + ":" + line + "\n";
    this->mergeFile->write( result.toUtf8() );
    this->mergeFile->flush();

}

qint64 MasterBufferManager::bytesDownloaded( int blockSize )
{

    qDebug () << " \n\n ============= IN BYTES DOWNLOAD =============== \n ";
    qDebug () <<  " merge Index = " << this->mergeIndex ;

    int fileSize = this->mergeIndex * blockSize;
    qDebug () <<  " File Size  = " << fileSize;

    for( int i = 0 ; i < this->indexList.size(); i ++ ) {

          int index = this->indexList[i];
          QString indexFilePath = this->dListItem->segmentDir + QString::number( index );
          QFileInfo fileInfo ( indexFilePath);
          if( fileInfo.exists() ) {
             fileSize += fileInfo.size();
             qDebug () << " filesize = " << fileSize;
          }

    }
    return fileSize;
    qDebug () << " \n\n ====================================================";

}
