#include "propertydialog.h"
#include "ui_propertydialog.h"

PropertyDialog::PropertyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertyDialog)
{
    ui->setupUi(this);
}

PropertyDialog::~PropertyDialog()
{
    delete ui;
}

void PropertyDialog::init(DownloadListItem *dListItem)
{

    this->dListItem = dListItem;
    this->show();


    QFileInfo info(this->dListItem->getAbsoluteFilePath(false));
    QFileIconProvider ip;
    QIcon icon=ip.icon(info);
    this->ui->file_icon->setPixmap( icon.pixmap( 50, 50));


    this->ui->lbl_filename->setText( this->dListItem->filename );
    this->ui->lbl_size->setText( this->dListItem->getFilesizeStr());

    //int status = this->dListItem-

            /*
    if( status == DownloadList::DOWNLOADING  ) {
        model->setItem(j, 1, new QStandardItem( QString:: number ( percentCompleted, 'f', 2) + " %") );
    } else {
        model->setItem(j, 1, new QStandardItem( this->dList->statusString[ status ] ));
    }
*/


    this->ui->lbl_filepath->setText( this->dListItem->getAbsoluteFilePath(true));

    this->ui->current_url->setText( this->dListItem->currentUrl );
    this->ui->url->setText( this->dListItem->url );



}

void PropertyDialog::on_pushButton_clicked()
{

    QString filePath = this->dListItem->getAbsoluteFilePath( false);
    QFile file( filePath );
    if( !file.exists() ) {
        QMessageBox::warning( this, " File Not Found ", " File Not Found " + filePath );
        return;
    }

    hc = new HelperClasses();
    hc->openFolder( filePath );
}

void PropertyDialog::on_pushButton_3_clicked()
{
    this->close();
}

void PropertyDialog::on_pushButton_2_clicked()
{
    QString filePath = this->dListItem->getAbsoluteFilePath(false);
    QDesktopServices::openUrl( "file:///" + filePath );
}
