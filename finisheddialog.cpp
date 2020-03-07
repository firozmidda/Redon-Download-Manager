#include "finisheddialog.h"
#include "ui_finisheddialog.h"

FinishedDialog::FinishedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinishedDialog)
{
    ui->setupUi(this);
    setWindowFlags( Qt::Dialog   | Qt::WindowMinimizeButtonHint    | Qt::WindowCloseButtonHint    | Qt::CustomizeWindowHint );
}

void FinishedDialog::init(DownloadListItem *dListItem)
{


    this->dListItem = dListItem;
    this->setWindowTitle( "Download Complete ");
    this->show();

    QFileInfo info(this->dListItem->getAbsoluteFilePath(false));
    QFileIconProvider ip;
    QIcon icon=ip.icon(info);
    this->ui->file_icon->setPixmap( icon.pixmap( 50, 50));

    QString filenameAndSize = this->dListItem->filename + "\n" + this->dListItem->getFilesizeStr();
    this->ui->lbl_filename->setText( filenameAndSize );
    this->ui->le_url->setText( this->dListItem->currentUrl );
    this->ui->le_saved_path->setText( this->dListItem->getAbsoluteFilePath( true ));



 /*
    QFileInfo info(this->dListItem->downloadRoot + this->dListItem->filename);
    QFileIconProvider ip;
    QIcon icon=ip.icon(info);

    this->ui->label_2->setPixmap( icon.pixmap(50, 50));
   */

    /*

    this->ui->lbl_filename->setText( this->dListItem->filename );
    this->ui->lbl_size->setText( this->dListItem->getFilesizeStr());
    this->ui->lbl_url->setText( this->dListItem->currentUrl );
    this->ui->lbl_saveas->setText( this->dListItem->downloadRoot + this->dListItem->filename );
    */



}

FinishedDialog::~FinishedDialog()
{
    delete ui;
}

void FinishedDialog::on_btn_open_clicked()
{
    QString filePath = this->dListItem->downloadRoot + this->dListItem->filename;
    QDesktopServices::openUrl( "file:///" +  filePath);

}

void FinishedDialog::on_btn_openFolder_clicked()
{    
    QString filePath = this->dListItem->getAbsoluteFilePath( false );

    QFile file( filePath );

    if( !file.exists()) {
        QMessageBox::warning( this, " File Not Found ", " File Not Found " + filePath);
        return ;
    }

    hc = new HelperClasses();
    hc->openFolder( filePath );

}

void FinishedDialog::on_cancel_clicked()
{
    this->close();
}
