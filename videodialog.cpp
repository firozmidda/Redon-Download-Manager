#include "videodialog.h"
#include "ui_videodialog.h"

#include<QDebug>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>



VideoDialog::VideoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoDialog)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Dialog   | Qt::WindowMinimizeButtonHint    | Qt::WindowCloseButtonHint    | Qt::CustomizeWindowHint );

    this->ui->widget_2->setVisible( false );
    this->ui->btn_download->setEnabled( false );
    this->vItemIndex = 0 ;

    QClipboard *clip =  QApplication::clipboard();
    QString url(clip->text());
    if( isValidUrl(url)) {
        this->ui->lineEdit->setText( url );
    }

    this->fillItagList();

    this->loaderMovie = new QMovie (":/other/icon/loader.gif");
    this->ui->lbl_loader->setMovie( this->loaderMovie );


    this->setWindowTitle( " Download Videos  ");


}

void VideoDialog::addVideoItem(VideoItem *item)
{
    qDebug () << " VideoDialog:  Item = " << this->vItemIndex;
    item->index =  this->vItemIndex;
    this->vItemList[ this->vItemIndex ++] = item;
}



void VideoDialog::fillItagList()
{
    this->itagHash.insert(5,    "FLV, 240p" );
    this->itagHash.insert(6,    "FLV, 270p");
    this->itagHash.insert(17,   "3GP, 144p" );
    this->itagHash.insert(18,   "MP4, 360p");
    this->itagHash.insert(22,   "MP4, 720p" );
    this->itagHash.insert(34,   "FLV, 360p");
    this->itagHash.insert(35,   "FLV, 480p");
    this->itagHash.insert(36,   "3GP, 240p");
    this->itagHash.insert(37,   "MP4, 1080p");
    this->itagHash.insert(38,   "MP4, 3072p");
    this->itagHash.insert(43,   "WebM, 360p");
    this->itagHash.insert(44,   "WebM, 480p");
    this->itagHash.insert(45,   "WebM, 720p");
    this->itagHash.insert(46,   "WebM, 1080p");
    this->itagHash.insert(82,   "MP4, 360p 3D");
    this->itagHash.insert(83,   "MP4, 240p 3D");
    this->itagHash.insert(84,   "MP4, 720p 3D");
    this->itagHash.insert(85,   "MP4, 520p 3D");
    this->itagHash.insert(100,	"WebM, 360p 3D");
    this->itagHash.insert(101, 	"WebM, 360p 3D");
    this->itagHash.insert(102, 	"WebM, 720p 3D");
    this->itagHash.insert(120, 	"FLV, 720p");
    this->itagHash.insert(133, 	"MP4, 240p");
    this->itagHash.insert(134, 	"MP4, 360p");
    this->itagHash.insert(135,  "MP4, 480p");
    this->itagHash.insert(136, 	"MP4, 720p");
    this->itagHash.insert(137, 	"MP4, 1080p");
    this->itagHash.insert(160, 	"MP4,  144p");

}

void VideoDialog::resetAll()
{
    this->vItemIndex = 0;
    this->ui->comboBox->clear();
    this->ui->lbl_title->clear();

}

void VideoDialog::init(QString url)
{
    this->ui->lineEdit->setText( url );
    this->on_btn_go_clicked();



    // the user sees it.

   ::SetWindowPos((HWND)this->effectiveWinId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   ::SetWindowPos( (HWND)this->effectiveWinId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   // HACK END


    this->show();

    this->activateWindow();
    this->raise();


}




QByteArray reverse(QByteArray& seq)
{
QByteArray reverse;
for( QByteArray::const_iterator i = seq.constEnd(); i !=seq.constBegin(); )
{
--i;
reverse += *i;
}
return reverse;
}


QByteArray cz( QByteArray a, int b) {

    char c = a[0];
    a[0] = a[b % a.length() ];
    a[b] = c;
    return a;

}

QString decypherSignature ( QString str ) {


    QByteArray a = str.toUtf8();

    a = cz ( a, 61)   ;
    a = cz ( a, 5);
    a = reverse( a );
    a = a.mid( 2);
    a = cz ( a, 69 );
    a = a.mid( 2 );
    a = reverse ( a);

    return QString (a );

}

QString getYoutubeVideoId ( QString url ) {

    QString videoId = "";
    QStringList urlList = url.split("v=");
    if( urlList.length() > 1 ) {
        videoId = url.split("v=")[1].split("&")[0];
    }
    return videoId;

}


void VideoDialog::startProcessing(QNetworkReply *rep)
{

    if( rep->error() != QNetworkReply::NoError) {
        QMessageBox::warning( this, " Error Occured ", rep->errorString() );
        return;
    }



    QJsonDocument document = QJsonDocument::fromJson( rep->readAll() );
    QJsonObject object = document.object();

    QJsonValue value = object.value("link");
    QString title = object.value("title").toString();
    title.replace( QRegExp("[^a-zA-Z0-9]"),QString("_"));
    this->filename = title;


    if( value.isArray() ) {
        qDebug () << " Value is Array  ";
    } else if ( value.isObject()) {
        qDebug () << " Value Is Object ";

        QJsonObject linkObject = value.toObject();
        QVariantMap tmpMap =  linkObject.toVariantMap();


        QStringList keys = tmpMap.keys();

        for( int i = 0 ; i < keys.length(); i ++) {


            QStringList dataList = tmpMap.value( keys[i]).toStringList();
            QString url = dataList[0] + "&title=" + title;

            QString itag = keys[i];

            int itagNum = itag.toInt();

            QString videoInfo = " , ";
            if( this->itagHash.contains(itagNum)) {
                videoInfo = this->itagHash[ itagNum ];
            }

            VideoItem *vItem = new VideoItem( itag, url , videoInfo);
            connect ( vItem, SIGNAL(updateComboBox(int)), this, SLOT(updateComboBox(int)));
            vItem->getSize();
            this->addVideoItem( vItem);


        }


    }

    this->ui->widget_2->setVisible( true );
    this->ui->lbl_title->setText( title );

    this->updateComboBox(-1);
    this->ui->btn_download->setEnabled( true );
    this->ui->lbl_loader->setVisible( false );
    this->loaderMovie->stop();








/*


    QMap<QString, QString> result = this->parse( data );

    QString status = result["status"];

    if( status != "ok") {
        QString reason = QUrl::fromPercentEncoding( result["reason"].toUtf8() ).replace ('+', " ");
        QMessageBox::warning( this, " Error Occured  ", reason);
        return ;
    }

    QString title = QUrl::fromPercentEncoding(result["title"].toUtf8()).replace('+', " ");
    title.replace( QRegExp("[^a-zA-Z0-9]"),QString("_"));
    this->filename = title;


    QString streamMap = QUrl::fromPercentEncoding(  result["url_encoded_fmt_stream_map"].toUtf8() );

    QStringList formatArr =  streamMap.split(',');

    for ( int i = 0 ; i <formatArr.length(); i++) {
        QMap<QString, QString> formatResult = this->parse( formatArr[i] );



        QString itag = formatResult["itag"];
        QString url = QUrl::fromPercentEncoding( formatResult["url"].toUtf8());
        QString sig = formatResult["sig"];
        if( sig == "") {
        //    qDebug () << " BEfore Cypher " << formatResult["s"];
            sig = formatResult["s"];
          //  qDebug () << " After  Cypher " << sig << "\n\n\n ";
        }

        url = url + "&signature=" + sig + "&title=" + title ;

        int itagNum = itag.toInt();

        QString videoInfo = " , ";
        if( this->itagHash.contains(itagNum)) {
            videoInfo = this->itagHash[ itagNum ];
        }

        VideoItem *vItem = new VideoItem( itag, url , videoInfo);
        connect ( vItem, SIGNAL(updateComboBox(int)), this, SLOT(updateComboBox(int)));
        vItem->getSize();
        this->addVideoItem( vItem);

    }

    this->ui->widget_2->setVisible( true );
    this->ui->lbl_title->setText( title );

    this->updateComboBox(-1);
    this->ui->btn_download->setEnabled( true );
    this->ui->lbl_loader->setVisible( false );
    this->loaderMovie->stop();
*/

}

QMap<QString, QString> VideoDialog::parse(QString data)
{
    QStringList parseStr = data.split('&');
    int len = parseStr.length();
    QMap<QString, QString> result;
    for( int i = 0 ; i < len; i ++) {
        QStringList tmpList = parseStr[i].split('=');
        result.insert(tmpList[0],  tmpList[1].toUtf8());
    }
    return result;
}


VideoDialog::~VideoDialog()
{
    delete ui;
}




void VideoDialog::replyFinished(QNetworkReply *rep)
{

}

void VideoDialog::error(QNetworkReply::NetworkError err)
{
    qDebug () << " Error Occured "<<this->reply->errorString();

}

void VideoDialog::updateComboBox(int i)
{
    if( i < 0 ) {
        for( int i = 0 ; i < this->vItemIndex; i ++) {
            QString str = this->vItemList[i]->getInfo();
            this->ui->comboBox->addItem( str );
        }
    } else {

          QString str = this->vItemList[i]->getInfo();
          this->ui->comboBox->setItemText(i, str);

    }

}

void VideoDialog::showThumbnail(QByteArray jpegData)
{
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    this->ui->lbl_thumbnail->setPixmap( pixmap );
}



void VideoDialog::on_btn_go_clicked()
{

    this->resetAll();
    this->ui->widget_2->setVisible( true );
    this->loaderMovie->start();
    this->ui->lbl_loader->setVisible( true);


    this->url = this->ui->lineEdit->text().trimmed();

    if( this->url.contains("youtube.com") ) {


        QString videoId = getYoutubeVideoId( this->url );


        if( videoId.length() > 1  ) {

            QString infoUrl = "http://ytapi.gitnol.com/get.php?id=" + videoId+"&apikey=13251dsf1sdfsdgs31gf1gfr5thth555";



            this->manager = new QNetworkAccessManager();
            this->reply = this->manager->get( QNetworkRequest( infoUrl ) );

            connect ( this->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
            connect ( this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(startProcessing(QNetworkReply*)) );

            this->thumbnail = new VideoThumbnail(this);
            connect ( this->thumbnail, SIGNAL(gotThumbnail(QByteArray)), this, SLOT(showThumbnail(QByteArray)));

            this->thumbnail->init( this->url);


        } else {

            QMessageBox::warning( this, " Invalid Video Id  ", " Please Enter Valid  Video  Id ");
            this->resetAll();
        }


    } else {
       QMessageBox::warning( this, " Invalid Url ", " Please Enter Youtube Video Url ");
        this->resetAll();
    }



}

void VideoDialog::on_btn_download_clicked()
{
     int index = this->ui->comboBox->currentIndex();
     VideoItem *item =  this->vItemList[index];
     DownloadListItem dListItem;

     int start = filename.lastIndexOf('.');
     QString filenameOnly = "";
     QString ext = "";
     if( start > 1) {
         QString tmpFilename = filename.mid(0, start);
         ext = filename.mid( start + 1);
         filenameOnly = tmpFilename;
     }


     dListItem.multipart = true; // later check and change..
     dListItem.filenameOnly =  filenameOnly;
     dListItem.extension = ext;
     dListItem.filename = this->filename;

     dListItem.url = item->url;
     dListItem.filesize = item->size;

     emit doDownload( dListItem );
     this->close();

}

void VideoDialog::on_btn_cancel_clicked()
{
    this->close();
}


/*========================================================
 *          V I T E M
==========================================================*/

VideoItem::VideoItem(QObject *parent) :
    QObject(parent)
{
    this->itag = "";
    this->url = "";
    this->ext = "";
    this->quality = "";

    this->size = 0;

}

VideoItem::VideoItem(QString itag, QString url , QString videoInfo)
{
    this->itag = itag;
    this->url = url;
    this->size = 0;
    QStringList tmpStr = videoInfo.split(',');
    this->ext = tmpStr[0];
    this->quality = tmpStr[1];

}

void VideoItem::getSize()
{
    this->manager = new QNetworkAccessManager();
    QNetworkRequest request( this->url );
    this->reply = this->manager->head( request);
    connect ( this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(sizeReceived(QNetworkReply*)));

}

QString VideoItem::getInfo()
{
    QString str = "";

    str = this->ext + " " + this->quality;
    if( this->size > 0 ) {
        str += "  " + sizeToString( this->size );
    }

    return str;
}

void VideoItem::sizeReceived(QNetworkReply *rep)
{
    this->size = rep->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    emit updateComboBox( this->index );
}

/*=======================================================
 *          T H U M B N A I L
 =========================================================*/

VideoThumbnail :: VideoThumbnail(QObject *parent ) :
    QObject(parent)
{
    this->manager = new QNetworkAccessManager(this);
}



void VideoThumbnail::init(QString url)
{
   QString videoId = url.split("v=")[1].split("&")[0];
   if( url != "") {

       QString imgUrl = "https://i1.ytimg.com/vi/" + videoId + "/default.jpg";
       this->reply = this->manager->get( QNetworkRequest( imgUrl ) );
       connect ( this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(imageReceived(QNetworkReply*)) );

   }
}

void VideoThumbnail::imageReceived(QNetworkReply *rep) {

   emit gotThumbnail( rep->readAll() );
}
