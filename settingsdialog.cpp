#include "settingsdialog.h"
#include "ui_settingsdialog.h"


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QStringList list;
    for( int i = 1 ; i < 9; i ++) {
        list << QString :: number( i );
    }
    this->ui->cb_connection->addItems( list );
    this->ui->cb_connection->setCurrentIndex( 7 );

}

void SettingsDialog::init(ConfigSettings *settings)
{
    this->cSettings = settings;

    if (  settings->maxConnections >=0  && settings->maxConnections < 9 ) {
        this->ui->cb_connection->setCurrentIndex( settings->maxConnections - 1 );
    }


    this->ui->tb_document->setText( settings->SDocument ) ;
    this->ui->tb_compressed->setText( settings->SCompressed ) ;
    this->ui->tb_music->setText( settings->SMusic ) ;
    this->ui->tb_video->setText( settings->SVideo ) ;
    this->ui->tb_picture->setText( settings->SPicture ) ;
    this->ui->tb_app->setText( settings->SApplication ) ;

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButton_clicked()
{
    // 1. save settings to config settings structure;
    // 2. call save settings


    // save the settings
    this->cSettings->setConnection( this->ui->cb_connection->currentIndex() + 1 );
    this->cSettings->setFileTypes( this->ui->tb_document->toPlainText(),
                                   this->ui->tb_compressed->toPlainText(),
                                   this->ui->tb_music->toPlainText(),
                                   this->ui->tb_video->toPlainText(),
                                   this->ui->tb_picture->toPlainText(),
                                   this->ui->tb_app->toPlainText()
                                   );


    //this->cSettings

    this->cSettings->saveSettings();
    this->close();



}

void SettingsDialog::on_pushButton_2_clicked()
{
    this->close();
}
