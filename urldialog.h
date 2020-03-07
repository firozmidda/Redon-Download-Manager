#ifndef URLDIALOG_H
#define URLDIALOG_H

#include <QDialog>
#include <QHostInfo>
#include "downloadlist.h"

namespace Ui {
class UrlDialog;
}

class UrlDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit UrlDialog(QWidget *parent = 0);
    ~UrlDialog();


signals:
    void doDownload( DownloadListItem );

    
private slots:
    void on_btn_download_clicked();
    void on_btn_cancel_clicked();

private:
    Ui::UrlDialog *ui;


};

#endif // URLDIALOG_H
