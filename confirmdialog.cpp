#include "confirmdialog.h"
#include "ui_confirmdialog.h"

ConfirmDialog::ConfirmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmDialog)
{

    ui->setupUi(this);

    this->setWindowFlags( Qt::Dialog   | Qt::WindowMinimizeButtonHint    | Qt::WindowCloseButtonHint    | Qt::CustomizeWindowHint  );
    this->setWindowTitle(" Confirm Download ");
    this->dListItem = new DownloadListItem();

    this->movie = new QMovie(":/other/icon/loader.gif");
    this->ui->lbl_loading->setMovie( this->movie );

    this->dlg = new QFileDialog();
    this->reply = NULL;

    this->localFilename = "";

    /** ----------- FOR SPEED CHECKING ------------*/
    this->speedCheckIndex = 0 ;
    for( int i = 0 ; i < 10;  i ++) {
        this->speedCheckArray[i] = 0;
    }
    this->speedTimer = new QTime() ;




}

ConfirmDialog::~ConfirmDialog()
{
    delete ui;
}

void ConfirmDialog::init(QString url, DownloadList *dList, SystemData *sysData, ConfigSettings *cSettings)
{
/*
    this->url = url;
    this->dList = dList;
    this->sysData = sysData;
    this->cSettings = cSettings;

    this->ui->lbl_url->setText( this->url );

    this->displayLoader();


    this->downloadPath = this->sysData->defaultDownloadPath + dList->categoryString[1] + "/";
    this->ui->lineEdit->setText( this->downloadPath  );

    this->dlg->setDirectory( this->ui->lineEdit->text() );


    for( int i = DownloadList::GENERAL ; i <= DownloadList::CUSTOM ; i ++ ) {

          this->ui->comboBox->addItem(   dList->categoryString[i]);
    }

    this->ui->btn_start->setText( "  Fetching Information please Wait....  ");

    this->manager = new QNetworkAccessManager(this);
    this->dListItem->url = url;

    this->show();
    this->startProcess();
*/




    /** -------------- INITIALIZE LOCAL VARIABLES -------------------- */


    this->url = url;
    this->dList = dList;
    this->sysData = sysData;
    this->cSettings = cSettings;
    this->dListItem->url = url;

    // process the url
    QUrl qUrl(this->url);
    if( qUrl.scheme() == "")     {
        qDebug () << " no Scheme Exists ";
        this->url = "http://" + this->url;
    }

    /** ----------------- UPDATE GUI COMPONENT -------------------------------------*/
    this->ui->lbl_url->setText( this->url );
    //this->downloadPath = this->sysData->defaultDownloadPath + dList->categoryString[1] + "/" + this->localFilename;
    this->pathOnly = this->sysData->defaultDownloadPath + dList->categoryString[1] + "/";
    this->ui->lineEdit->setText( QDir::toNativeSeparators( this->pathOnly + this->localFilename ) );
    this->dlg->setDirectory( this->ui->lineEdit->text() );

    for( int i = DownloadList::GENERAL ; i <= DownloadList::CUSTOM ; i ++ ) {
          this->ui->comboBox->addItem(   dList->categoryString[i]);
    }

    this->ui->btn_start->setText( " Fetching Information please Wait... ");
    this->displayLoader();

    /** ----------------- START THE INIT PROCESS  ------------------------------*/


    // the user sees it.

   ::SetWindowPos((HWND)this->effectiveWinId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   ::SetWindowPos( (HWND)this->effectiveWinId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   // HACK END


    this->show();

    this->activateWindow();
    this->raise();

    this->startProcess();

}

void ConfirmDialog::startProcess()
{

      this->manager = new QNetworkAccessManager (this );
      QSslConfiguration cnf;
      cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
      cnf.setProtocol((QSsl::AnyProtocol));

      QNetworkRequest request( QUrl( this->url ));
      request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
      request.setPriority( QNetworkRequest ::HighPriority );
      request.setSslConfiguration(cnf);

      request.setRawHeader(qStr2Ba("range"), qStr2Ba("bytes=0-1"));


      this->speedTimer->start();
      this->reply = manager->get( request );

      connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT (lengthReceived (QNetworkReply *)));
      connect(this->reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(error(QNetworkReply::NetworkError)));
      connect(this->reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslError(QList<QSslError>)));


}

void ConfirmDialog::parseRawHeader(QNetworkReply *rep)
{

    qint64 len = 0;
    if( rep->rawHeader(qStr2Ba("Content-Range")).contains("bytes")) {
       QRegExp reg("bytes \\d-\\d/(\\d+)");
       QString cr = rep->rawHeader(qStr2Ba("Content-Range"));

       if (reg.indexIn(cr) > -1) {
           len = reg.cap(1).toInt();
           if( len > 0 ) {
               this->dListItem->filesize = len;
           }
        }
        this->dListItem->multipart = true;
    }

    this->dListItem->currentUrl = this->url;
    this->getFilename( rep );
    finished();

}

QString ConfirmDialog::getFilename(QNetworkReply *rep)
{

    QString filename = "";
    QString filenameOnly = "";
    QString extension = "";

    QString contentType = rep->header( QNetworkRequest :: ContentTypeHeader ).toString().trimmed();


    // Displaying the Header
    /*
    QList<QByteArray> headerList =  rep->rawHeaderList();
    for( int i = 0; i< headerList.length(); i++) {
        qDebug () << headerList[i] << " = " << rep->rawHeader( headerList[i] );
    }
*/

    QUrl qurl = rep->url();
    QString url = qurl.toString();

    QFileInfo fileInfo(qurl.path());
    filename = fileInfo.fileName();
    qDebug () <<" \n\n filename = "<< filename;


    if( qurl.hasQuery()  ) {
        QUrlQuery urlq(qurl);
        if( urlq.hasQueryItem("title") ) {
            filename = urlq.queryItemValue("title", QUrl::FullyDecoded);
            filename = filename.replace( QRegExp("[^a-zA-Z0-9]"),QString("_"));
        }
    }


    if( filename == "") {
        int start = url.indexOf("://");
        if( start > 0 ) {
            start = start + 3;
            int end = url.indexOf("/", start + 1);
            filename = url.mid(start, end - start);
            end  = filename.lastIndexOf('.');

            if( end > 0 ) {
                filename = filename.mid(0, end);
            }

            filename.replace(".", "_");
        }
    }

    // http://127.0.0.1/arnav/02%20Love%20Dose%20(Desi%20Kalakaar)%20Yo%20Yo%20Honey%20Singh%20190Kbps.mp3
    // http://node5.promptfile.com/flv/1/32/bcd1d2fc93e68442f1b45cc458ddf091e3efcfa9_rf.flv?h=7QW_EYBxxPyZs9EdAOtwsA&e=1414538474&f=Into.The.Storm.2014.HDRip.XviD.AC3EVO.avi.avi
    qDebug () << " IN GET FILENAME = " << filename;

    // Seperate Filename and Extension
    int end = filename.lastIndexOf(".");

    if( end < 0) {
        end = filename.length();
    }

    filenameOnly = filename.mid(0, end);
    extension = filename.mid( end + 1);

    qDebug () << " Filename only = " << filenameOnly;
    qDebug () << " Ext = " << extension;
    qDebug () << " Content Type =  " << contentType;


    if( extension == "") {
         extension = this->sysData->getExtension( contentType );
         qDebug () << " Extension = " << extension;
    }



    if( extension == "" && contentType.indexOf("html")) {
        extension = "html";
    }

    filenameOnly = filenameOnly.replace( QRegExp("[^a-zA-Z0-9]"),QString("_"));

    this->dListItem->mimeType = contentType;
    this->dListItem->filenameOnly = filenameOnly;
    this->dListItem->extension = extension;
    this->dListItem->filename = filenameOnly + "." + extension;

    int comboBoxindex = this->checkFileType( extension );
    this->ui->comboBox->setCurrentIndex( comboBoxindex );


    /*
    bool appendExt = false;
    QString filename = "";
    QUrl qurl = rep->url();
    QString url = qurl.toString();

    QString contentType = rep->header( QNetworkRequest :: ContentTypeHeader ).toString().trimmed();
    qDebug () << "content type = " << contentType << " url = " << url;
    QString ext = this->sysData->getExtension( contentType );
    qDebug () << "===== ext = " << ext;

    int comboBoxIndex = this->checkFileType(ext);
    this->ui->comboBox->setCurrentIndex( comboBoxIndex );


    this->dListItem->extension = ext;
    this->dListItem->mimeType = contentType;


    if( qurl.hasQuery()  ) {

        qDebug () << " Has Query ";
        QUrlQuery urlq(qurl);
        filename = urlq.queryItemValue("title", QUrl::FullyDecoded);
        appendExt = true;
    }

    if( filename == "") {

        qDebug () << " Filename is Empty ";

        QFileInfo fileInfo(qurl.path());
        filename = fileInfo.fileName();

        int start = filename.lastIndexOf('.') ;
        if( start > 1 ) {
            QString tmpFilename = filename.mid(0, start);
            ext = filename.mid(start + 1);
            filename = tmpFilename;
        }


    }

    if( filename == "" &&  (contentType.indexOf("html") != -1)) {

        qDebug () << " Here 3  ";

        int start = url.indexOf("://");
        if( start > 0 ) {
            start = start + 3;
            int end = url.indexOf("/", start + 1);
            filename = url.mid(start, end - start);

            end  = filename.lastIndexOf('.');
            if( end > 0 ) {
                filename = filename.mid(0, end);
            }
           filename.replace(".", "_");
        }
        appendExt = true;
    }

    filename.replace( QRegExp("[^a-zA-Z0-9]"),QString("_"));

    this->dListItem->filenameOnly = filename;

    if( appendExt == true || (contentType.indexOf("html") != -1)  ) {
        filename = filename + "."+ext;
        this->dListItem->extension = ext;
    }

    qDebug () << " FILENAME ONLY  = " << filename;


    this->dListItem->extension = ext;
    this->dListItem->mimeType = contentType;

    this->dListItem->filename = filename;

    */

    return filename;

}

int ConfirmDialog::checkFileType(QString ext)
{
    qDebug () << "confirm Dialog : ext = " << ext;

    if(  this->cSettings->HDocument.contains( ext ) ) {
        return DownloadList::DOCUMENT;

    } else if (  this->cSettings->HMusic.contains( ext ) ) {
        return DownloadList::MUSIC;

    } else if (  this->cSettings->HVideo.contains( ext ) ) {
        return DownloadList::VIDEO;

    } else if (  this->cSettings->HPicture.contains( ext ) ) {
        return DownloadList::PICTURE;

    } else if (  this->cSettings->HCompressed.contains( ext ) ) {
        return DownloadList::COMPRESSED;

    } else if (  this->cSettings->HApplication.contains( ext ) ) {
        return DownloadList::APPLICATION;
    }

    return 0;

}

void ConfirmDialog::finished()
{

/*
     qDebug() <<" Filename Only = " << this->dListItem->filenameOnly;
    qDebug() <<" Filename = " << this->dListItem->filename;
    qDebug() <<" Extension  = " << this->dListItem->extension;

    qDebug() <<" multipart = " << this->dListItem->multipart;
    qDebug() <<" length = " << this->dListItem->filesize;
    qDebug() <<" url = " << this->dListItem->url;
    qDebug() <<" current url = " << this->dListItem->currentUrl;


    this->ui->btn_start->setEnabled( true );
    this->ui->btn_start->setText( " Start Download ");
    this->ui->lbl_size->setText( sizeToString (this->dListItem->filesize ));
    this->setWindowTitle(  this->dListItem->filename );

    this->hideLoader();

*/


    /** --------------  UPDATE GUI --------------------------*/
    this->ui->btn_start->setEnabled( true );
    this->ui->btn_start->setText(" Start Download ");
    this->ui->lbl_size->setText( sizeToString (this->dListItem->filesize ));
    this->setWindowTitle(  this->dListItem->filename );

    this->processFileExistance();

    QFileInfo fileInfo ( this->ui->lineEdit->text().trimmed() );
    QString path = fileInfo.absolutePath() + "/";

    qDebug () << " Path = " << path;
    qDebug () << " Local filanem = " << this->localFilename;

    path = path + this->localFilename;
    this->ui->lineEdit->setText( QDir::toNativeSeparators( path) );

    this->hideLoader();

}

void ConfirmDialog::displayLoader()
{
    this->movie->start();
}

void ConfirmDialog::hideLoader()
{
    this->movie->stop();
    this->ui->lbl_loading->setVisible( false );
}

void ConfirmDialog::processFileExistance()
{
    this->fixPath();
    QFileInfo fileInfo ( this->ui->lineEdit->text() );
    QString path = fileInfo.absolutePath() + "/";

    QString filenameOnly = this->dListItem->filenameOnly;
    QString ext = this->dListItem->extension;

    if ( filenameOnly != "") {

        this->localFilename = filenameOnly + "." + ext;

        if( QFile ( path + this->localFilename ).exists() ) {
            this->localFilename =  this->generateNewFilename( path, filenameOnly, ext);
        }
    }

}



QString ConfirmDialog::generateNewFilename(QString path, QString filenameOnly, QString ext)
{
    int i = 2;

    QString result = "";
    while ( TRUE ) {

        QString filename = filenameOnly + "_" + QString::number( i ) +"." + ext;
        QString fullFilename = path +  filename;

        if( QFile ( fullFilename ).exists() ) {
            i ++;
            continue;
        } else {
            result = filename;
            break;
        }
    }

    if( i > 2 ) {
        this->sysData->redonUsageData->incFRCount();
    }

    return result;
}

void ConfirmDialog::fixPath()
{


     qDebug () << " \n\n  ================ FIX PATH =================== \n";
     QString downloadRoot = QDir::toNativeSeparators( this->pathOnly );
     qDebug () << " DOWNLOAD ROOT = " << downloadRoot;
     if( downloadRoot != "") {

         int lastPosition = downloadRoot.lastIndexOf('\\');
         int len = downloadRoot.length();

         if( lastPosition != len - 1  ) {
             downloadRoot = downloadRoot + '/';
         }

         this->ui->lineEdit->setText( QDir::toNativeSeparators(downloadRoot + this->localFilename) );
     }
     qDebug () << " DOWNLOAD ROOT = " << downloadRoot;
     qDebug () << " \n\n  ================ FIX PATH ENDS =================== \n";

}





/* ==============================================================
 *      P U B L I C   S L O T S
===================================================================*/

void ConfirmDialog::readReady()
{
#ifdef QT_DEBUG
    qDebug () << "CONFIRM DIALOG  Read Ready " ;
#endif

}

void ConfirmDialog::error(QNetworkReply::NetworkError err)
{

#ifdef QT_DEBUG
    qDebug () << " CONFIRM DIALOG Error Occured " <<err ;
#endif
}

void ConfirmDialog::sslError(QList<QSslError>)
{
#ifdef QT_DEBUG
    qDebug () << " CONFIRM DIALOG SSL Error Occured "  ;
#endif

}




/* ==============================================================
 *      P R I V A T E     S L O T S
===================================================================*/
void ConfirmDialog::on_pushButton_clicked()
{

    QString newLocation = dlg->getExistingDirectory( this, " Please Select Folder ");
    newLocation = newLocation.trimmed();

    if( newLocation != "") {
        //this->downloadPath = newLocation;
        this->pathOnly = newLocation;
        this->ui->lineEdit->setText( this->downloadPath );
    }

    this->processFileExistance();

    this->ui->comboBox->setCurrentIndex( DownloadList ::CUSTOM );
    this->dListItem->downloadRoot = this->pathOnly;


    QString path = this->pathOnly + this->localFilename;
    this->ui->lineEdit->setText( QDir::toNativeSeparators( path ) );

    /*

    QString newLocation = dlg->getExistingDirectory( this, " Please Select Folder ");
    newLocation = newLocation.trimmed();
    this->processFileExistance();

    if( newLocation != "") {
        this->downloadPath = newLocation;
        this->ui->lineEdit->setText( this->downloadPath + this->localFilename);
    }

    this->ui->comboBox->setCurrentIndex( DownloadList::CUSTOM  );
    this->dListItem->downloadRoot = this->downloadPath;
    */

}

void ConfirmDialog::on_comboBox_currentIndexChanged(int index)
{

   if( (index  ) !=  DownloadList::CUSTOM ) {

       QString path = this->sysData->defaultDownloadPath + this->dList->categoryString[index] + "/";
       this->pathOnly = path;
       this->ui->lineEdit->setText( QDir::toNativeSeparators( path ));
       this->processFileExistance();

       path = this->pathOnly + this->localFilename;
       this->ui->lineEdit->setText( QDir::toNativeSeparators( path ) );
   }

}

void ConfirmDialog::on_pushButton_3_clicked()
{
    this->sysData->redonUsageData->incDCCount();
    this->close();
}

void ConfirmDialog::on_btn_start_clicked()
{


    QString downloadPath = this->ui->lineEdit->text().trimmed();


    qDebug () << " Download Path " <<  downloadPath;
    if( downloadPath == "" ) {
        QMessageBox :: warning( this, " Please Select Save as Directory ", " Please Select Location to download this file ");
        return;
    }

    if ( QFile(downloadPath).exists()) {
        QMessageBox :: warning( this, " Save As ", " File Already Exists. It Will Overwrite The file ");
        return ;
    }


    QFileInfo fileInfo ( downloadPath );
    QString downloadRoot = fileInfo.absolutePath() + "/";

    qDebug () << " Download Root = " << downloadRoot;
    if( !QDir ( downloadRoot ).exists() ) {
        QMessageBox :: warning( this, " Save As ", " Selected Directory Does not Exists. Please Select Valid One  ");
        return ;
    }

    /** ---------------------------- CALCULATE AVERAGE SPEED ------------- */
    int total = 0;
    for( int i = 0; i < this->speedCheckIndex; i ++ ) {
        total += this->speedCheckArray[i];
    }

    int averageSpeed = total / this->speedCheckIndex;
    qDebug () << "\n\n\n ========= CURRENT Average  SPEED ==========" << "\n" << averageSpeed
              << "= " << sizeToString(averageSpeed)   << "\n\n\n";

    this->dListItem->downloadRoot = downloadRoot;
    this->dListItem->category = this->ui->comboBox->currentIndex();
    this->dListItem->filename = this->localFilename;
    emit downloadConfirmed( this->dListItem );
    this->close();


    qDebug () << " download Root " << this->dListItem->downloadRoot;
    qDebug () << " filename " << this->dListItem->filename;

    /*
    QString downloadRoot = this->ui->lineEdit->text().trimmed();
    if( downloadRoot == "") {
        QMessageBox :: warning( this, " Please Select Save as Directory ", " Please Select Location to download this file ");
        return;
    }


    if ( QDir( downloadRoot ).exists() == false ) {
        QMessageBox :: warning( this, " Save As ", " Selected Directory Doesnot Exists. Please Select Valid One  ");
        return ;
    }

    int lastPosition = downloadRoot.lastIndexOf('/');
    int len = downloadRoot.length();

    if( lastPosition != len - 1  ) {
        downloadRoot = downloadRoot + "/";
    }

    this->dListItem->downloadRoot = downloadRoot;
    this->dListItem->category = this->ui->comboBox->currentIndex();
    qDebug () << " Current Index = " << this->ui->comboBox->currentIndex();

    */

}


void ConfirmDialog::lengthReceived(QNetworkReply *rep)
{
    qDebug () << " Length Receied ";

    if( rep->error() == QNetworkReply::NoError) {

        /** --------------- CHECK SPEED FIRST ----------------*/
        qint64 timeElapsed = this->speedTimer->elapsed();
        int speed = ( rep->bytesAvailable() * 1000 )  /  timeElapsed;
        this->speedCheckArray[ this->speedCheckIndex ++] = speed;


        this->url = rep->url().toString();
        this->ui->lbl_url->setText( this->url );
        QString location = rep->header( QNetworkRequest::LocationHeader ).toString().trimmed();

        if( location == "" ) {

          //  disconnect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT (lengthReceived (QNetworkReply *)));
            qDebug () << " \n\n\ PARSINg HEADER  \n\n ";

            this->parseRawHeader( this->reply );


        } else {
            qDebug () << " \n\n\n LOCATION \n\n " ;
            //qDebug () << " LOCATION = " << location;
            this->url = location;
            this->startProcess();
        }


    } else {
         QMessageBox::warning( this, " Error occured ", rep->errorString() );
    }





}
