#ifndef VIDEODIALOG_H
#define VIDEODIALOG_H

#include <QtCore>
#include <QtGui>
#include <QDialog>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonDocument>
#include "downloadlist.h"



#include "lib.h"


class VideoThumbnail : public QObject
{
    Q_OBJECT
public:
    explicit VideoThumbnail(QObject *parent = 0);
    void init ( QString url );

    QNetworkAccessManager *manager;
    QNetworkReply *reply;


signals:
    void gotThumbnail(QByteArray data);


public slots:
  void imageReceived(QNetworkReply *rep);

};


class VideoItem : public QObject
{
    Q_OBJECT
public:
    explicit VideoItem(QObject *parent = 0);
    VideoItem ( QString itag,  QString url , QString videoInfo);

    QString itag;
    QString url;
    QString ext;
    QString quality;
    qint64 size;

    int index;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QAbstractTableModel *model;

    void getSize();
    QString  getInfo();


signals:
    void updateComboBox(int i);

public slots:
  void sizeReceived(QNetworkReply *rep);

};



namespace Ui {
class VideoDialog;
}

class VideoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VideoDialog(QWidget *parent = 0);
    void addVideoItem( VideoItem *item);
    void fillItagList();
    void resetAll();
    void init( QString url);

    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QHash<int, QString> itagHash;
    QMovie *loaderMovie;

    ~VideoDialog();


signals:
    void doDownload( DownloadListItem dListItem );

public slots:
    void replyFinished(QNetworkReply*);
    void error(QNetworkReply::NetworkError err);
    void updateComboBox(int i);
    void showThumbnail(QByteArray jpegData);

    void startProcessing(QNetworkReply* rep);
    QMap <QString, QString>parse (QString data);

private slots:
    void on_btn_go_clicked();
    void on_btn_download_clicked();
    void on_btn_cancel_clicked();

private:
    Ui::VideoDialog *ui;

    QString url;
    QString filename;


    VideoItem *vItemList[20];
    VideoThumbnail *thumbnail;
    int vItemIndex;

};

#endif // VIDEODIALOG_H
