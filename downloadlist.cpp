#include "downloadlist.h"



/*========================================================
 *          DOWNLOAD LIST ITEM
==========================================================*/

DownloadListItem::DownloadListItem()
{
    this->filesize = 0;
    this->multipart = false;
    this->timeLeft = 0;
    this->currentSpeed = 0;
    this->percentCompleted = 0;
    this->downloadStatus = DownloadList::PENDING;
    this->tModel = NULL;
    this->active = false;
    //this->createdDateTime =
}

DownloadListItem *DownloadList::getItem(int i)
{
    return this->dListCollection[i];
}

int DownloadList::length()
{
    return this->dListCollection.size();
}


QString DownloadListItem::getFilesizeStr()
{
    return sizeToString( this->filesize );
}

QString DownloadListItem::getAbsoluteFilePath(bool flag)
{
    QString path = this->downloadRoot  + this->filename;
    if( flag == true) { // in case of windows path
        path = path.replace('/', "\\");
    }
    return path;
}

void DownloadListItem::setDownloadStatus(int status)
{
    qDebug () << " SETTING DOWNLOAD STATUS = " << status;
    this->downloadStatus = status;
}

int DownloadListItem::getDownloadStatus()
{
    return this->downloadStatus;
}


/*========================================================
 *          DOWNLOAD LIST
==========================================================*/

DownloadList::DownloadList()
{


    this->statusString[ PENDING   ]     = "Pending";
    this->statusString[ DOWNLOADING ] = "Downloading";
    this->statusString[ PAUSED  ]      = "Paused";
    this->statusString[ STOPPED ]   = "Stopped";
    this->statusString[ COMPLETED ]   = "Completed";

    this->categoryString[ COMPRESSED ]  = SCAT_COMPRESSED_STR;
    this->categoryString[ DOCUMENT ]    = SCAT_DOCUMENT_STR;
    this->categoryString[ MUSIC ]       = SCAT_MUSIC_STR;
    this->categoryString[ PICTURE ]     = SCAT_PICTURE_STR;
    this->categoryString[ GENERAL ]     = SCAT_GENERAL_STR;
    this->categoryString[ VIDEO ]       = SCAT_VIDEO_STR;
    this->categoryString[ APPLICATION ] = SCAT_APPLICATION_STR;
    this->categoryString[ CUSTOM ]      = SCAT_CUSTOM_PATH_STR;



}

void DownloadList::loadList()
{

    QFile xmlFile("list.xml");
    if( !xmlFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QDomDocument document;
    document.setContent( &xmlFile );

    QDomElement root = document.firstChildElement();
    QDomNodeList items = root.elementsByTagName("list");

    int len = items.count();
    for ( int i = 0 ;  i < len; i++) {
        DownloadListItem *item = new DownloadListItem();

        QDomNode node =  items.at(i);
        QDomElement elem = node.toElement();

        item->multipart = false;
        item->url = elem.attribute("url");
        item->currentUrl = elem.attribute("current_url");
        item->filename = elem.attribute("filename");
        item->mimeType = elem.attribute("mime_type");
        item->filesize = elem.attribute("filesize").toLongLong();
        item->downloadRoot = elem.attribute("download_root");
        item->category = elem.attribute("category").toInt();

        item->setDownloadStatus( elem.attribute("download_status").toInt() );
        item->currentSpeed = elem.attribute("current_speed").toInt();
        item->timeLeft = elem.attribute("time_left").toLongLong();
        item->percentCompleted = elem.attribute("completed_percent").toFloat();
        item->segmentDir = elem.attribute("segment_dir");
        int value =  elem.attribute("multipart").toInt();
        if( value) {
            item->multipart = true;
        }

        this->addItem( item);

    }

}

void DownloadList::saveList()
{

       QFile xmlFile("list.xml");
       if( !xmlFile.open(QIODevice::WriteOnly)) {
           return;
       }
       QDomDocument document;
       QDomElement root = document.createElement("root");
     ///  qDebug () << " save List : list size =  " << this->dListCollection.size();
       document.appendChild( root );
       int len = this->dListCollection.size();
     ///  qDebug () << " save List : len =   " <<len;
       DownloadListItem *item;

       for( int i = 0; i < len;  i++ ) {
           item = this->dListCollection [i];
           QDomElement node = document.createElement("list");

           //qDebug () << " URL = " << item->url;

           node.setAttribute("url", item->url );
           node.setAttribute("current_url", item->currentUrl );
           node.setAttribute("filename", item->filename );
           node.setAttribute("mime_type", item->mimeType);

           node.setAttribute("filesize", item->filesize );
           node.setAttribute("download_root", item->downloadRoot);
           node.setAttribute("category", item->category);

           node.setAttribute("download_status", item->getDownloadStatus() );

           node.setAttribute("time_left", item->timeLeft);


           node.setAttribute("current_speed", item->currentSpeed );
           node.setAttribute("completed_percent", item->percentCompleted );
           node.setAttribute("segment_dir", item->segmentDir);
           node.setAttribute("multipart", item->multipart);


           qDebug () << " download status =  " << item->getDownloadStatus();



           root.appendChild( node );

       }


       QTextStream stream( &xmlFile );
       stream << document.toString();
       xmlFile.close();
       qDebug() << " File Closed " ;

}

void DownloadList::removeFromList(DownloadListItem *dListItem)
{

    qDebug  () << " Removing From Dlist  " << dListItem << " list size = " << this->dListCollection.size();
    int deletePos = -1;


    for( int i = 0 ; i < this->dListCollection.size(); i ++) {
        if( this->dListCollection[i] == dListItem ) {
            deletePos = i;
        }
    }

    this->dListCollection.removeOne( dListItem );

    qDebug () << " DELETE POS  = " << deletePos;
    /*
    if( deletePos >= 0 ) {
        this->dListCollection.removeAt( deletePos);
    }*/

    //this->dListCollection.removeAt( deletePos);

    /*
    for( int i = deletePos; i  < this->listSize ;  i ++ ) {
        this->dListCollection[i] = this->dListCollection[i + 1];
    }
    this->listSize -- ;
*/

    /*
    for( int i = 0 ; i < this->listSize; i ++ ) {
        DownloadListItem *item = this->dListCollection[i];
        if( this->dListCollection[i] == dListItem  ) {
            delete item;
            DownloadListItem *item = this->dListCollection[i + 1];
            i++;
        }
        this->dListCollection[i] = item;
    }*/


}

void DownloadList::refereshList(int parentType, int childType)
{

    this->parentId = parentType;
    this->childId = childType;

}

void DownloadList::filterList()
{
    qDebug() << " filter List parent id = " << this->parentId << " child id = " << this->childId;

    this->tmpListSize = 0 ;
    this->tmpListCollection.clear();

    if ( this->parentId == 0 ) {

        if( this->childId == -1) {
            for( int i = 0 ;  i < this->dListCollection.size(); i ++) {

               this->tmpListCollection.append( this->dListCollection[i] );
               this->tmpListCollection[ this->tmpListSize ++ ]->active = true;

               //this->tmpListCollection[ this->tmpListSize ++] = this->dListCollection[i];
              // this->tmpListCollection[ this->tmpListSize -1 ]->active = true;

            }
        } else {

            for( int i = 0 ;  i < this->dListCollection.size(); i ++) {
                if( this->dListCollection[i]->category == this->childId ) {
                    this->tmpListCollection.append( this->dListCollection[i] );
                    this->tmpListCollection[ this->tmpListSize ++ ]->active = true;

                } else {
                    this->dListCollection[i]->active = false;
                }
            }

        }

    } else if (  this->parentId == 1) {

        if( this->childId == -1) {
            for( int i = 0 ;  i < this->dListCollection.size(); i ++) {
                if( this->dListCollection[i]->getDownloadStatus() == COMPLETED ) {
                    this->tmpListCollection.append( this->dListCollection[i] );
                    this->tmpListCollection[ this->tmpListSize ++ ]->active = true;

                } else {
                    this->dListCollection[i]->active = false;
                }
            }
        } else {

            for( int i = 0 ;  i < this->dListCollection.size(); i ++) {
                if( this->dListCollection[i]->category == this->childId  && this->dListCollection[i]->getDownloadStatus() == COMPLETED ) {
                    this->tmpListCollection.append( this->dListCollection[i] );
                    this->tmpListCollection[ this->tmpListSize ++ ]->active = true;

                } else {
                    this->dListCollection[i]->active = false;
                }
            }

        }



    } else if ( this->parentId == 2) {

        if( this->childId == -1) {
            for( int i = 0 ;  i < this->dListCollection.size(); i ++) {
                if( this->dListCollection[i]->getDownloadStatus() != COMPLETED ) {
                    this->tmpListCollection.append( this->dListCollection[i] );
                    this->tmpListCollection[ this->tmpListSize ++ ]->active = true;

                } else {
                    this->dListCollection[i]->active = false;
                }
            }
        } else {

            for( int i = 0 ;  i < this->dListCollection.size(); i ++) {
                if( this->dListCollection[i]->category == this->childId  && this->dListCollection[i]->getDownloadStatus() != COMPLETED ) {
                    this->tmpListCollection.append( this->dListCollection[i] );
                    this->tmpListCollection[ this->tmpListSize ++ ]->active = true;
                } else {
                    this->dListCollection[i]->active = false;
                }
            }

        }


    }
    this->createTableModel();

    qDebug () << " Filter List: list size =  " << this->dListCollection.size();
}

void DownloadList::createTableModel()
{

    for( int i = 0 ; i < this->tmpListCollection.size(); i ++) {
        DownloadListItem *item = this->tmpListCollection[i];


        QFileInfo info( item->getAbsoluteFilePath(false));
        QFileIconProvider ip;
        QIcon icon = ip.icon(info);


        TableModel *tModel = new TableModel( item->currentUrl, item->filename, icon );
        if(  item->getDownloadStatus() == DownloadList::DOWNLOADING  ) {
           tModel->status =   new QStandardItem( QString:: number ( item->percentCompleted, 'f', 2) + " %");
        } else {
            tModel->status = new QStandardItem( this->statusString[ item->getDownloadStatus() ] );
        }

        tModel->size = new QStandardItem ( sizeToString( item->filesize  ));
        tModel->transferRate = new QStandardItem( sizeToString(  item->currentSpeed ) + "/s");
        tModel->savedPath = new QStandardItem ( item->getAbsoluteFilePath(true) );
        tModel->timeLeft = new QStandardItem ( timeToString( item->timeLeft ) );



        item->tModel = tModel;

    }


}

int DownloadList::addItem(DownloadListItem *dListItem)
{
    this->dListCollection.append( dListItem );
    /*
    this->dListCollection[ this->listSize ++] = dListItem;
    qDebug () << " DOWNLOAD STATUS = " << dListItem->getDownloadStatus();
    */
    return 0;
}

