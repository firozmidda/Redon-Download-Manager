#include "loaderwindow.h"
#include "ui_loaderwindow.h"

LoaderWindow::LoaderWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoaderWindow)
{

  //  qDebug () << " In Loader " << " option = " << option << " option Value = " << optionValue;
    QStringList releaseTypeArr;
    releaseTypeArr << " Alpha " << " Beta " << " Stable ";

    ui->setupUi(this);

    this->timer = new QTimer(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //hide window title bar
    this->ui->lbl_version->setText ( APPLICATION_VERSION + releaseTypeArr [ RELEASE_TYPE  - 1]);

}

LoaderWindow::~LoaderWindow()
{
    delete ui;
}

void LoaderWindow::init(MainWindow *mw, QString option, QString optionValue)
{

    this->mw = mw;
    this->sysData = mw->sysData;
    this->dList = mw->dList;
    this->option = option;
    this->optionValue = optionValue;

    if( option == "startup" || option == "url") {
        this->hide();
    }


    this->sysData->checkDefaultDownloadPath();
    this->sysData->checkDownloadDirs( this->dList );
    this->sysData->checkTmpDir();

    QString mimeFilename = QApplication::applicationDirPath() + "/mimeType.red";
    fileDebug (" MimeFIlename = " + mimeFilename + "\n");
    fileDebug (" Calling loadMimeData \n");
    if( this->sysData->loadMimeData( mimeFilename ) ) {
        this->sysData->errorFlag = true;
        QMessageBox::warning(this, " Error ", " Cannot Load Mime File " +  mimeFilename );
    }

    if( option == "startup" || option == "url") {
        this->timer->singleShot( 1 , this, SLOT(loadWindow()));
    } else {
        this->timer->singleShot( 500, this, SLOT(loadWindow()) );
    }



}

void LoaderWindow::loadWindow()
{



    this->mw->init( this->option, this->optionValue);
    this->close();

    /*
    qDebug () << " option = " << option;
    if( option.contains("onstart")) {
        this->mw->showMinimized();
    } else {
         this->mw->show();
    }
*/


}
