#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include <QDialog>
#include "downloadlist.h"
#include <QProcess>
#include "helperclasses.h"
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDesktopServices>
#include <QMessageBox>


namespace Ui {
class PropertyDialog;
}

class PropertyDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PropertyDialog(QWidget *parent = 0);
    ~PropertyDialog();
    void init (DownloadListItem *dListItem );
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::PropertyDialog *ui;
    DownloadListItem *dListItem;
    QProcess *process;
    HelperClasses *hc;
};

#endif // PROPERTYDIALOG_H
