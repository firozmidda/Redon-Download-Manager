#ifndef HELPERCLASSES_H
#define HELPERCLASSES_H

#include <QStandardItemModel>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include "helperclasses.h"
#include <QMessageBox>
#include <QMutex>


class TableModel
{
public:
    TableModel(QString url, QString filename, QIcon icon );


    QStandardItem *filename;
    QStandardItem *status;
    QStandardItem *size;
    QStandardItem *transferRate;
    QStandardItem *savedPath;
    QStandardItem *url;


    void setTimeLeft( QString timeleft );
    QStandardItem * getTimeLeft ();


    inline void setStatus (QString status) {
        if ( this->status->model()) {
            this->status->setText( status );
        }
    }





    QStandardItem *timeLeft;
};



struct segmentInfo {
    qint64 initial;
    qint64 current;
    qint64 end;
};


class DownloadState {

    QString filename;

    QFile *stateFile;
    QTextStream *textStream;

    QFile *tmpFile;
    QTextStream *tmpStream;


public:

    DownloadState(QString filename);
    void open();
    void flush();
    void close();
    void init (segmentInfo si[], int len  );
    void readSegment(segmentInfo si[], int *len);

    void writeSegment(int index, qint64 current);
    void writeFullSegmentInfo(int index, qint64 initial, qint64 current, qint64 end);
    void reopen();
    /*
    DownloadState(QString filename);
    void init (segmentInfo si[], int len  );
    void reopen ();
    void writeFullSegmentInfo(int index, qint64 initial, qint64 current, qint64 end);
    void writeSegment(int index, qint64 current);
    void flush();
    void close();


    void readSegment(segmentInfo si[], int *len);
    */


};




class HelperClasses
{
    public:
        HelperClasses();

     QProcess *process;
     void openFolder(QString filepath);

};

#endif // HELPERCLASSES_H
