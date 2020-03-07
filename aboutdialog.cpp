#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    QStringList releaseTypeArr;
    releaseTypeArr << " Alpha " << " Beta " << " Stable ";
    QString versionText = " v" + QString (APPLICATION_VERSION);
    versionText += releaseTypeArr [ RELEASE_TYPE - 1];

    ui->setupUi(this);


    this->ui->lbl_icon->setPixmap(QPixmap(":/app_icon/icon/app_icon.png", 0, Qt::AutoColor));
    this->ui->lbl_version->setText( versionText );

    this->setWindowTitle( "About Redon " );

}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    this->close();
}
