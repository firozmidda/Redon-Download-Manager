#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "configsettings.h"
#include<QDebug>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    void init ( ConfigSettings *settings);
    ~SettingsDialog();
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::SettingsDialog *ui;
    ConfigSettings *cSettings;
};

#endif // SETTINGSDIALOG_H
