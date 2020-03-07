#ifndef FINISHEDDIALOG_H
#define FINISHEDDIALOG_H

#include <QDialog>
#include <QtCore>
#include "downloadlist.h"
#include <QProcess>
#include <QPixmap>
#include <QIcon>
#include <QFileInfo>
#include <QFileIconProvider>
#include "helperclasses.h"
#include <QMessageBox>




namespace Ui {
class FinishedDialog;
}

class FinishedDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FinishedDialog(QWidget *parent = 0);
    void init(DownloadListItem *dListItem);
    ~FinishedDialog();
    
private slots:
    void on_btn_open_clicked();

    void on_btn_openFolder_clicked();

    void on_cancel_clicked();

private:
    Ui::FinishedDialog *ui;
    DownloadListItem *dListItem;
    HelperClasses *hc;

};

#endif // FINISHEDDIALOG_H
