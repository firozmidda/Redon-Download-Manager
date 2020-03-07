#ifndef DOWNLOADLIST_H
#define DOWNLOADLIST_H
#include <QtCore>
#include <QtXml>
#include "lib.h"
#include <helperclasses.h>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>



#define CAT_ALL_STR           "All"
#define CAT_FINISHED_STR      "Finished"
#define CAT_UNFINISHED_STR    "Unfinished"

#define SCAT_GENERAL_STR      "General"
#define SCAT_DOCUMENT_STR     "Documents"
#define SCAT_MUSIC_STR        "Music"
#define SCAT_VIDEO_STR        "Videos"
#define SCAT_PICTURE_STR      "Picture"
#define SCAT_COMPRESSED_STR   "Compressed"
#define SCAT_APPLICATION_STR  "Applications"
#define SCAT_CUSTOM_PATH_STR  "Custom Path"

#define _SPEED 51200
#define _MAXCONNECTION  8


#define MAX_BLOCK_SIZE (_SPEED * 5)
#define MIN_BLOCK_SIZE 1024

/*
struct downloadListItem {
    QString url;
    QString currentUrl;
    QString filename;
    QString fileType;
    qint64 filesize;
    QString createdDateTime;
    QString completedDateTime;

    int category;
    int status;
    int completedStatus;
    qint64 timeTaken;
    QString segmentDir;
    QString filepath;
    int averageSpeed;
    int currentSpeed;
    float completedPercent;

};
*/

class DownloadListItem {

 public:
/* =====================================================
 *          PROPERTIES
=========================================================*/

    QString url;
    QString currentUrl;
    QString filenameOnly;
    QString extension;
    QString filename;

    qint64 filesize;
    bool multipart;
    QString mimeType;
    int category;


    QString downloadRoot;
    QString segmentDir;



    int currentSpeed;
    float percentCompleted;
    qint64 timeLeft;
    QString createdDateTime;

    TableModel *tModel;
    bool active;

    /** ===============================================
     *      METHODS
    ==================================================*/
    DownloadListItem();
    QString getFilesizeStr ();
    QString getAbsoluteFilePath(bool flag);
    void setDownloadStatus ( int status );
    int getDownloadStatus ();




private:
    int downloadStatus;




};


class DownloadList
{
public:
    enum DOWNLOAD_CATEGORY { GENERAL = 0, DOCUMENT,  MUSIC,   VIDEO, PICTURE,  COMPRESSED, APPLICATION, CUSTOM };
    enum DOWNLOAD_STATUS { PENDING = 0 , DOWNLOADING, PAUSED,  STOPPED, COMPLETED };
    DownloadList();


    void loadList();
    void saveList();
    void removeFromList(DownloadListItem *dListItem);

    void refereshList( int parentType, int childType );
    void filterList();
    void createTableModel();

    QString statusString[10];
    QString categoryString[10];

    int addItem(DownloadListItem *dListItem );
    DownloadListItem *getItem( int i);
    int length();




    /** =============================================
     *      LIST IO
     =================================================*/
   // DownloadListItem *dListCollection[100];
    //DownloadListItem *tmpListCollection[100];

    QList <DownloadListItem *> dListCollection;
    QList <DownloadListItem *> tmpListCollection;

    int parentId;
    int childId;



    int tmpListSize;
private:



};

#endif // DOWNLOADLIST_H
