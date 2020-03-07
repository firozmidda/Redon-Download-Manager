#include "urldialog.h"
#include "ui_urldialog.h"
#include "lib.h"

UrlDialog::UrlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UrlDialog)
{
    ui->setupUi(this);
    QClipboard *clip = QApplication::clipboard();
    QString url( clip->text() );
    if( isValidUrl(url)) {
        this->ui->lineEdit->setText( url );
    }
    this->setWindowTitle( " Enter Address to Download " );

}

UrlDialog::~UrlDialog()
{
    delete ui;
}

void UrlDialog::on_btn_download_clicked()
{
    QString url = this->ui->lineEdit->text().trimmed();
    DownloadListItem dListItem;
    dListItem.url = url;
    emit doDownload( dListItem );
    this->close();
}

void UrlDialog::on_btn_cancel_clicked()
{
    this->close();
}
