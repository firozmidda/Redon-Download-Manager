#ifndef LOADERWINDOW_H
#define LOADERWINDOW_H

#include <QDialog>
#include "mainwindow.h"
#include "downloadlist.h"
#include "configsettings.h"
#include <QMessageBox>
#include "lib.h"

namespace Ui {
class LoaderWindow;
}

class LoaderWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoaderWindow(QWidget *parent = 0);
    ~LoaderWindow();
    void init (MainWindow *mw, QString option, QString optionValue);


public slots:
    void loadWindow();


private:
    MainWindow *mw;
    SystemData *sysData;
    ConfigSettings *cSettings;
    DownloadList *dList;
    QTimer *timer;
    QString option;
    QString optionValue;

    Ui::LoaderWindow *ui;
};

#endif // LOADERWINDOW_H
