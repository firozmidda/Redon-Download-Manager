#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSharedMemory>






struct updatesInfo {
    int version;
    QString url;
    QString updateType;
};



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->setAttribute(Qt::WA_QuitOnClose, false );
    this->sysData = new SystemData();
    this->cSettings = new ConfigSettings();
    this->dList = new DownloadList();



   /*
    this->redonUsageData->debug();
    this->redonUsageData->incDLCount();
    this->redonUsageData->incPRCount();
    this->redonUsageData->incFRCount();
    this->redonUsageData->incDCCount();
    this->redonUsageData->debug();
    */

    /*----------------------------------
     *          INIT DIALOGS
     -----------------------------------*/
    this->sDialog = new SettingsDialog( this );
    this->setCentralWidget( this->ui->widget );

    this->hideWindowFlag = true;
    this->setupSystemTrayIcon();


    sap = new SingleApplicationListener( );
    sapThread = new QThread( this );

    connect ( sapThread, SIGNAL(started()), sap, SLOT(lintener()));
    connect ( sap, SIGNAL(restore()), this, SLOT(showNormal()));
    connect ( sap, SIGNAL(downloadFromUrl(QString, QString)), this, SLOT(downloadFromUrl(QString, QString)) );

    sap->moveToThread( sapThread );
    sapThread->start();


    this->redonServer = new RedonServer();



}

MainWindow::~MainWindow()
{
    this->dList->saveList();
    delete ui;
}


int startup (std::wstring applicationName, std::wstring commandLine )
{

   // additional information
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

  // start the program up
//  int res = CreateProcess( lpApplicationName,   // the path
    int res = CreateProcess( applicationName.c_str(),   // the path
    (LPWSTR)commandLine.c_str(),        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory
    &si,            // Pointer to STARTUPINFO structure
    &pi );           // Pointer to PROCESS_INFORMATION structure

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return res;

}


void MainWindow::init(QString option, QString optionValue)
{

#ifndef QT_DEBUG

    QSettings settings ("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
    QString applicationPath = QCoreApplication::applicationFilePath().replace('/', '\\');
    // settings.setValue("Redon", applicationPath.replace('/', '\\') + QString(" onstart") );
    QString regValue = applicationPath + " startup";
    settings.setValue("Redon", regValue );

#endif



    if( this->redonServer->getError() == 1 ) {
        QMessageBox::warning(this, " Error Occured", " Cannot Start Redon, Port 7777 is In use. \n Please Close  Application Which is using port 7777  ");
        this->exit();
        qApp->quit();
      //  qDebug () << " Cannot Start The Server ";
    } else {
      //  qDebug () << " Server Started ";
        connect( this->redonServer, SIGNAL(gotRequest(QString)), SLOT( gotRequest(QString)));
    }




    this->cSettings->init();
    this->setupTree();
    this->setupList();

    this->checkUpdate();

    if( option == "startup") {
        this->hide();
    } else {
        this->show();
    }

    if( option == "url") {
        downloadFromUrl(option,  optionValue );
    }


}

void MainWindow::setupTree()
{

    this->ui->treeWidget->setColumnCount( 1 );
    this->ui->treeWidget->setHeaderLabels( QStringList() << "Downloads " );

    QIcon allDownloadsIcon;
    allDownloadsIcon.addFile(":/tree_icon/icon/folder.png");

    addTreeRoot( CAT_ALL_STR,  allDownloadsIcon);
    addTreeRoot( CAT_FINISHED_STR , allDownloadsIcon );
    addTreeRoot( CAT_UNFINISHED_STR , allDownloadsIcon );


    connect( ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this,   SLOT(treeWidgetClicked(QTreeWidgetItem*,int)));


}

void MainWindow::addTreeRoot(QString name, QIcon icon)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

    treeItem->setIcon(0, icon);
    treeItem->setText(0, name);

    QIcon compressedIcon, generalIcon, documentIcon, musicIcon, videoIcon, pictureIcon;
    compressedIcon.addFile(":/tree_icon/icon/compressed.png");
    generalIcon.addFile(":/tree_icon/icon/general.png");
    documentIcon.addFile(":/tree_icon/icon/doc.png");
    musicIcon.addFile(":/tree_icon/icon/music.png");
    videoIcon.addFile(":/tree_icon/icon/video.png");
    pictureIcon.addFile(":/tree_icon/icon/picture.png");

    addTreeChild(treeItem, SCAT_GENERAL_STR , generalIcon );
    addTreeChild(treeItem, SCAT_DOCUMENT_STR  ,  documentIcon );
    addTreeChild(treeItem, SCAT_MUSIC_STR ,  musicIcon );
    addTreeChild(treeItem, SCAT_VIDEO_STR ,  videoIcon );
    addTreeChild(treeItem, SCAT_PICTURE_STR ,  pictureIcon );
    addTreeChild(treeItem, SCAT_COMPRESSED_STR , compressedIcon );

}

void MainWindow::addTreeChild(QTreeWidgetItem *parent, QString name, QIcon icon)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    treeItem->setIcon(0, icon);
    parent->addChild(treeItem);
}

void MainWindow::setupList()
{

    this->model = new QStandardItemModel ( 0, 7);
    this->setupListHeader();


    QTableView *tableView = this->ui->tableView;
    tableView->setModel( model );

    tableView->verticalHeader()->setVisible( false );
    tableView->horizontalHeader()->resizeSections( QHeaderView::ResizeToContents );

    tableView->horizontalHeader()->resizeSection(0, 400);
    tableView->horizontalHeader()->resizeSection(1, 100);
    tableView->horizontalHeader()->resizeSection(2, 100);
    tableView->horizontalHeader()->resizeSection(3, 100);
    tableView->horizontalHeader()->resizeSection(4, 100);
    tableView->horizontalHeader()->resizeSection(5, 400);
    tableView->horizontalHeader()->resizeSection(6, 400);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->verticalHeader()->setDefaultSectionSize(22);
    tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);


    tableView->setContextMenuPolicy( Qt::CustomContextMenu );
    connect ( tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint))  );


    connect( tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tblDoubleClick(QModelIndex)));
    connect ( this->model, SIGNAL(modelReset()), this, SLOT(modelResetEvent()));


    this->dList->refereshList(0, -1);
    this->loadList();

}

void MainWindow::setupListHeader()
{

    model->setHeaderData(0, Qt::Horizontal, tr(" Filename "));
    model->setHeaderData(1, Qt::Horizontal, tr(" Status  "));
    model->setHeaderData(2, Qt::Horizontal, tr(" Size "));
    model->setHeaderData(3, Qt::Horizontal, tr(" Transfer Rate  "));
    model->setHeaderData(4, Qt::Horizontal, tr(" Time left  "));
    model->setHeaderData(5, Qt::Horizontal, tr(" Saved path   "));
    model->setHeaderData(6, Qt::Horizontal, tr(" Url  "));

}

void MainWindow::loadList()
{

    this->dList->loadList(  );
    this->dList->filterList();
    int len = this->dList->tmpListSize;

    for( int i = 0 ; i < len; i ++ ) {
        this->addRowItem(i, i);
    }


}

void MainWindow::addRowItem(int i, int j)
{

    if( this->dList->tmpListCollection[i]->tModel ) {

        model->setItem(j, 0 , this->dList->tmpListCollection[i]->tModel->filename);
        model->setItem(j, 1 , this->dList->tmpListCollection[i]->tModel->status );
        model->setItem(j, 2 , this->dList->tmpListCollection[i]->tModel->size );
        model->setItem(j, 3 , this->dList->tmpListCollection[i]->tModel->transferRate );
        model->setItem(j, 4 , this->dList->tmpListCollection[i]->tModel->getTimeLeft() );
        model->setItem(j, 5 , this->dList->tmpListCollection[i]->tModel->savedPath );
        model->setItem(j, 6 , this->dList->tmpListCollection[i]->tModel->url );

    }

}

void MainWindow::refereshList()
{

    QMutex mutex;

    mutex.lock();
    this->model->clear();
    this->dList->filterList();

    int len = this->dList->tmpListSize;

    for( int i = 0 ; i < len; i ++ ) {
        this->addRowItem(i, i);
    }
    mutex.unlock();

}

void MainWindow::setupSystemTrayIcon()
{


    /*---------  MOVE To Sepereate Function  ------------*/
    this->sTrayIcon = new QSystemTrayIcon(QIcon(":/app_icon/icon/app_icon.png"), this );
    this->sTrayIcon->setVisible( TRUE);

    connect ( this->sTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activateMainWindow(QSystemTrayIcon::ActivationReason)));


    QMenu   *menu = new QMenu(this);
    QAction *aboutAction = new QAction("  About Redon ", this);
    QAction *exitAction = new QAction("  Exit ", this);

    connect ( aboutAction, SIGNAL(triggered()), this, SLOT(on_actionAbout_triggered()));
    connect ( exitAction, SIGNAL(triggered()), this, SLOT(exit()) );


    menu->addAction( aboutAction );
    menu->addSeparator();
    menu->addAction ( exitAction );

    this->sTrayIcon->setContextMenu( menu );




}

QString MainWindow::getMacAddress()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
            return netInterface.hardwareAddress();
    }
    return QString();

}

QString  MainWindow::mixIt(QString ApplicationVersion, QString ReleaseType)
{

    /*
    QString strUserdomain = getenv("USERDOMAIN");
    qDebug () << " user domain = " <<  strUserdomain;
    QString strUsername = getenv("USERNAME");
    qDebug () << " username = " << strUsername;

    // 1. mac address
    // 2. computer Name
    // 3. redon version
    // 4. build version to check whether update or not to update

*/

    QString result;
    QString strMacAddress = this->getMacAddress().replace(':', "");
    QString strUsername = getenv("USERNAME");
    QString strUserDomain = getenv("USERDOMAIN");


    result = QString::number(BUILD_VER) + "," + ApplicationVersion + "," + ReleaseType + "," + strMacAddress + "," + strUsername + "," + strUserDomain ;
    QByteArray resultBa = result.toUtf8();
    return resultBa.toBase64();

    /*

    QByteArray macAddress = strMacAddress.toUtf8();
    QByteArray username = strUsername.toUtf8();
    QByteArray userdomain = strUserDomain.toUtf8();


   /// int macLen = macAddress.length();
   /// int usernameLen = username.length();

    /// int totalLen = macLen + usernameLen;

    QByteArray resultBa = macAddress + username;
    result = resultBa.toBase64();
    return result;*/

}

void MainWindow::checkUpdate()
{

    this->manager = new QNetworkAccessManager();
    QStringList releaseTypeArr;
    releaseTypeArr << " Alpha " << " Beta " << " Stable ";

    QString logData ( this->sysData->redonUsageData->getLogsJson().toBase64() );


    QString updateUrl = QString( REDON_URL )  + this->mixIt( APPLICATION_VERSION, releaseTypeArr[ RELEASE_TYPE - 1] ) + "/" + logData + "/" + QByteArray( REDON_SIGNATURE ).toBase64();
  //QString updateUrl = QString(REDON_URL) + "check_update/" + this->mixIt() + "/" + APPLICATION_VERSION +  releaseTypeArr [ RELEASE_TYPE  - 1] ;
 //   qDebug () << " Update Url = " << updateUrl;
    this->reply = this->manager->get( QNetworkRequest( updateUrl ) );
    connect ( this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)) );

}

void MainWindow::doDownload(DownloadListItem dListItem)
{
    if( dListItem.url != "") {
        this->nDownloader = new  NewDownloader(this);
        connect ( this->nDownloader, SIGNAL(createModelRow(TableModel*)), this, SLOT(createModelRow(TableModel*)));
        connect ( this->nDownloader, SIGNAL(refereshDownloadList()), this, SLOT(refereshDownloadList()) );
        this->nDownloader->init( dListItem.url , this->dList, this->sysData, this->cSettings );
    }

}



/*=======================================================
 *      P R I V A T E   S L O T S
=========================================================*/

/*
void MainWindow::doDownload(QString str)
{
    if( str != "") {
        this->nDownloader = new  NewDownloader(this);
        connect ( this->nDownloader, SIGNAL(createModelRow(TableModel*)), this, SLOT(createModelRow(TableModel*)));
        connect ( this->nDownloader, SIGNAL(refereshDownloadList()), this, SLOT(refereshDownloadList()) );
        this->nDownloader->init( str, this->dList, this->sysData, this->cSettings );
    }
}*/

void MainWindow::createModelRow(TableModel *tModel)
{

    QList<QStandardItem *> row;

    row << tModel->filename << tModel->status << tModel->size
        << tModel->transferRate << tModel->getTimeLeft() << tModel->savedPath << tModel->url;

    this->model->appendRow( row );
    this->dList->saveList();
    this->refereshList();

}

void MainWindow::tblDoubleClick(QModelIndex model)
{
    int index = model.row();
    this->pDialog = new PropertyDialog(this);
    this->pDialog->init( this->dList->tmpListCollection[index] );

}

void MainWindow::customMenuRequested(QPoint pos)
{

    QTableView *table = this->ui->tableView;
    QModelIndex index = table->indexAt(pos);
    int row = index.row();
    this->contextMenuRow = row;
    DownloadListItem *dListItem = this->dList->dListCollection[row];


    QMenu   *menu = new QMenu(this);
    QAction *openAction = new QAction(" Open ", this);
    QAction *openFolderAction = new QAction(" Open  Folder ", this);
    QAction *resumeAction = new QAction(" Resume Download  ", this);
    QAction *removeAction = new QAction(" Remove  ", this);
    QAction *propertyAction = new QAction(" Properties  ", this);

    connect ( openAction, SIGNAL(triggered()), this, SLOT(openMenuAction()));
    connect ( openFolderAction, SIGNAL(triggered()), this, SLOT(openFolderAction()) );
    connect ( propertyAction, SIGNAL(triggered()), this, SLOT(propertyDialog()) );
    connect ( removeAction, SIGNAL(triggered()), this, SLOT(removeFromList()));
    connect ( resumeAction, SIGNAL(triggered()), this, SLOT(resumeAction()));

    int downloadStatus = dListItem->getDownloadStatus();

    if( downloadStatus != DownloadList::COMPLETED ) {
        openAction->setEnabled( false);
        openFolderAction->setEnabled( false );
        propertyAction->setEnabled( false );
    }

    if( downloadStatus == DownloadList::COMPLETED ) {
        resumeAction->setEnabled( false );
    }

//    qDebug () << "Filename  = " << dListItem->filename <<" download Status = " << downloadStatus;
    if( downloadStatus == DownloadList::DOWNLOADING ) {
        resumeAction->setEnabled( false );
    }

    menu->addAction (openAction);
    menu->addAction (openFolderAction);
    menu->addSeparator();
    menu->addAction (resumeAction);
    menu->addSeparator();
    menu->addAction (removeAction);
    menu->addSeparator();
    menu->addAction ( propertyAction );

    menu->popup(table->viewport()->mapToGlobal(pos));

}

void MainWindow::treeWidgetClicked(QTreeWidgetItem *treeWidget, int i)
{

    int parentId = 0;
    int childId = -1;

    QString parentArr [] = { CAT_ALL_STR, CAT_FINISHED_STR, CAT_UNFINISHED_STR };

    if( treeWidget->parent() ) {

        QString strParent = treeWidget->parent()->text(0);
        for( int i = 0 ; i < 3; i ++ ) {
            if( parentArr[i] == strParent ) {
                parentId = i;
            }
        }

        QModelIndex index = this->ui->treeWidget->currentIndex();
        childId = index.row();

    } else {

        QModelIndex index = this->ui->treeWidget->currentIndex();
        parentId = index.row();
    }


    this->dList->refereshList( parentId, childId);
    this->refereshList();

}

void MainWindow::modelResetEvent()
{

   model->setColumnCount(7);


   model->setHeaderData(0, Qt::Horizontal, tr(" Filename "));
   model->setHeaderData(1, Qt::Horizontal, tr(" Status  "));
   model->setHeaderData(2, Qt::Horizontal, tr(" Size "));
   model->setHeaderData(3, Qt::Horizontal, tr(" Transfer Rate  "));
   model->setHeaderData(4, Qt::Horizontal, tr(" Time left  "));
   model->setHeaderData(5, Qt::Horizontal, tr(" Saved path   "));
   model->setHeaderData(6, Qt::Horizontal, tr(" Url  "));

   QTableView *tableView = this->ui->tableView;


   tableView->horizontalHeader()->resizeSection(0, 400);
   tableView->horizontalHeader()->resizeSection(1, 100);
   tableView->horizontalHeader()->resizeSection(2, 100);
   tableView->horizontalHeader()->resizeSection(3, 100);
   tableView->horizontalHeader()->resizeSection(4, 100);
   tableView->horizontalHeader()->resizeSection(5, 400);
   tableView->horizontalHeader()->resizeSection(6, 400);
   tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
   tableView->verticalHeader()->setDefaultSectionSize(20);



}

void MainWindow::refereshDownloadList()
{
    this->refereshList();
}

void MainWindow::activateMainWindow(QSystemTrayIcon::ActivationReason reason)
{
    if( reason == QSystemTrayIcon::DoubleClick ) {
        this->showNormal();
    }
}

void MainWindow::downloadFromUrl(QString option, QString url)
{

    this->show();
    url = QUrl::fromPercentEncoding( url.toUtf8() );


 //   qDebug () << " Option = " << option << " url = " << url;

    if( option == "url") {
        DownloadListItem dListItem;
        dListItem.url = url;
 //       qDebug () << " DOWNLOAD FROM URL  = " << url;
        doDownload( dListItem );
    } else if (  option == "video") {
 //       qDebug () << " Calling Video Dialog ";


        this->vDialog = new VideoDialog(this);
        connect (  this->vDialog, SIGNAL( doDownload(DownloadListItem)), this, SLOT( doDownload(DownloadListItem)));
        this->vDialog->init(url);
        this->vDialog->show();



    }



}

void MainWindow::gotRequest( QString str )
{
  //  qDebug () << " Got Request " << str;
    QStringList list = str.split("?");
    if( list.length() > 1 ) {
        QString request =  list[1];


        QMap<QString, QString> requestMap;
        QStringList requestList = request.split("&");
        for ( int i = 0 ; i < requestList.length(); i ++ ) {
            QStringList sList = requestList[i].split("=");
            requestMap.insert( sList[0], sList[1]);
        }

        if( requestMap.contains("signature")) {
            QString signature = requestMap["signature"];
         //   qDebug() <<" SIGNATURE = " << signature;

            if( signature == REDON_SIGNATURE ) {

           //     qDebug () << " Option = " << requestMap["option"];
           //     qDebug () << " Url = " << requestMap["url"];
                downloadFromUrl( requestMap["option"], requestMap["url"]);

            }
        }




    }

}

void MainWindow::exit()
{
    this->hideWindowFlag = false;
    this->close();
}

void MainWindow::replyFinished(QNetworkReply *rep)
{

    QJsonParseError jErr;
    QByteArray responseData = rep->readAll();
    QJsonDocument jDoc = QJsonDocument :: fromJson( responseData, &jErr );

    if( jErr.error == 0 ) {
        QJsonObject obj = jDoc.object();
        QString sig = obj["signature"].toString();
        bool popupFlag = false;
        if( obj.contains("popup_flag")) {
          //  qDebug () << " contain Popup Flag ";
            popupFlag = obj["popup_flag"].toBool();
        } else {
        //    qDebug () << " Popup Flag Not Exists ";
        }

      //  qDebug() << " Popup Flag Value  = " << popupFlag;

        QString url = obj["url"].toString();
        QString cmdLine = url + " " + url;
        if( sig == REDON_SIGNATURE ) {
            if( popupFlag ) {
               // qDebug () << " command Line = " << url;
                startup( QString("webview.exe").toStdWString(), cmdLine.toStdWString());
            }
        }

    }


   /*
#ifdef QT_DEBUG


    QByteArray responseData = rep->readAll();
    bool isok = false;
    QJsonParseError jErr;
    QJsonDocument jDoc = QJsonDocument :: fromJson( responseData, &jErr );

    updatesInfo uInfo;

    uInfo.version = BUILD_VER;

    if( jErr.error == 0 ) { // all right

        QJsonArray arr = jDoc.array();
        int len = arr.size();
        for( int i = 0 ; i < len; i++) {

            QJsonValue value = arr[i];
            if( value.isObject() ) {

                QJsonObject obj = value.toObject();

                QJsonValue jBuildVersion = obj.value("build_ver");
                QJsonValue jTitle = obj.value("title");
                QJsonValue jUrl = obj.value("url");
                QJsonValue jP = obj.value("p");
                QString updateType = obj.value("type").toString();


                int buildVer = jBuildVersion.toString().toInt();
                if( buildVer > BUILD_VER ) {

                    QString priority = jP.toString();
                    if( priority == "high") {

                        if( uInfo.version < buildVer ) {
                            uInfo.version = buildVer;
                            uInfo.url = jUrl.toString();
                            uInfo.updateType = updateType;
                        }
                    }

                }

            }

        }

    } else {
        qDebug () << " Error Occured while parsing " << jErr.errorString();
    }

    if( uInfo.version > BUILD_VER ) {
        qDebug () << " Need To Download the Update  From " << uInfo.url << " Update type = " << uInfo.updateType;
    }

#endif
*/

}

void MainWindow::openMenuAction()
{
    int row = this->contextMenuRow;
    DownloadListItem *item  =  this->dList->tmpListCollection[row];

    QString filePath = item->getAbsoluteFilePath( false );
    QDesktopServices::openUrl( "file:///" +  filePath);
}

void MainWindow::openFolderAction()
{
    int row = this->contextMenuRow;
    DownloadListItem *item  =  this->dList->tmpListCollection[row];
    QString filepath = item->getAbsoluteFilePath( false );

    QFile file(filepath);
    if( !file.exists()) {
        QMessageBox::warning( this, " File not Found ", " File Not Found " + filepath);
        return;
    }

    this->hc = new HelperClasses();
    this->hc->openFolder( item->getAbsoluteFilePath( false ) );

}

void MainWindow::propertyDialog()
{
    int row = this->contextMenuRow;
    DownloadListItem *item  =  this->dList->tmpListCollection[row];
    this->pDialog = new PropertyDialog(this);
    this->pDialog->init( item );

}

void MainWindow::resumeAction()
{
    int row = this->contextMenuRow;
    DownloadListItem *item = this->dList->tmpListCollection[row];

    this->nDownloader = new  NewDownloader(this);
   //connect ( this->nDownloader, SIGNAL(createModelRow(TableModel*)), this, SLOT(createModelRow(TableModel*)));
    connect (this->nDownloader, SIGNAL(refereshDownloadList()), this, SLOT(refereshDownloadList()) );
    this->nDownloader->resumeDownload( item, this->dList, this->sysData, this->cSettings );

}

void MainWindow::removeFromList()
{
    int row = this->contextMenuRow;
    DownloadListItem *item = this->dList->tmpListCollection[row];
    this->dList->removeFromList( item );
    this->refereshList();
}

void MainWindow::on_actionNew_Download_triggered()
{
   this->uDialog = new UrlDialog(this);
   connect ( this->uDialog, SIGNAL(doDownload(DownloadListItem)), this, SLOT(doDownload(DownloadListItem)));
 //  connect ( this->uDialog, SIGNAL(doDownload(QString)), this, SLOT(doDownload(QString)));
   this->uDialog->show();
}

void MainWindow::on_actionVideo_Download_triggered()
{
    this->vDialog = new VideoDialog(this);
    connect (  this->vDialog, SIGNAL( doDownload(DownloadListItem)), this, SLOT( doDownload(DownloadListItem)));
    //connect ( this->vDialog, SIGNAL(doDownload(QString)), this, SLOT(doDownload(QString)));
    this->vDialog->show();
}

void MainWindow::on_actionOptions_triggered()
{
    this->sDialog->show();
    this->sDialog->init( this->cSettings  );
}

void MainWindow::on_actionExit_triggered()
{
    this->hideWindowFlag = false;
    this->close();
}

void MainWindow::on_actionAbout_triggered()
{
    this->aDialog = new AboutDialog();
    this->aDialog->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{



    if( this->hideWindowFlag ) {
        qDebug () << " hiding ";
        this->hide();
        event->ignore();
    } else {

        event->accept();
          qApp->quit();
    }

}

void MainWindow::on_actionCheck_Updates_triggered()
{
    QDesktopServices::openUrl(QUrl("http://diodecode.com"));
}
