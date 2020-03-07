#include "mainwindow.h"
#include <QApplication>
#include "loaderwindow.h"
#include "applicationinfo.h"
#include <QSharedMemory>
#include "lib.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);


    QString option = "";
    QString optionValue = "";



    if( argc > 1 ) {
        option = QString ( argv[1]);
        if( argc > 2) {
            optionValue = QString(argv[2]);

        }
    }


    fileDebug( optionValue );

    if( option == "") {
        option = "start";
    }


    QString cmdLine = "\n argv 0 = " + QString(argv[0])+ " 1 = " + QString ( argv[1]) + " 2 = " + QString(argv[2]);

    fileDebug( cmdLine );

    a.processEvents();
    //79cfcd13-b295-48bd-aa42-6788f2b0a074
    QSharedMemory shared("79cfcd13-b295-48bd-aa42-6788f2b0a074");

    if( !shared.create( 1024, QSharedMemory::ReadWrite) )   {
        // For a GUI application, replace this by :
        // QMessageBox msgBox;
        //msgBox.setText( QObject::tr("Can't start more than one instance of the application.") );
        //msgBox.setIcon( QMessageBox::Critical );
        //msgBox.exec();

        QSharedMemory sharedMemory("singleApplication");

        sharedMemory.create(1024);
        sharedMemory.lock();
        char *to = (char*)sharedMemory.data();
        QString result = option + " " + optionValue;
       // qDebug () << " shared Memory result = " << result;
        memcpy(to, result.toUtf8(), result.length() );
        sharedMemory.unlock();

        exit (0);

   } else {
       qDebug() << "Application started successfully.";
   }


    //1. display The Loader Window

    LoaderWindow *lw = new LoaderWindow();
    lw->show();

    //3. start mainwindow


    a.setQuitOnLastWindowClosed( false);

    MainWindow w;


    lw->init( &w, option, optionValue);

    return a.exec();
}
