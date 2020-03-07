#-------------------------------------------------
#
#
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += network
QT       += sql


CONFIG += static

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = redon
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    loaderwindow.cpp \
    systemdata.cpp \
    downloadlist.cpp \
    settingsdialog.cpp \
    configsettings.cpp \
    urldialog.cpp \
    videodialog.cpp \
    lib.cpp \
    newdownloader.cpp \
    confirmdialog.cpp \
    downloaddialog.cpp \
    helperclasses.cpp \
    segmentrow.cpp \
    filedownloader.cpp \
    filejoiner.cpp \
    finisheddialog.cpp \
    propertydialog.cpp \
    aboutdialog.cpp \
    blockmanagementsystem.cpp \
    masterbuffermanager.cpp \
    workerspool.cpp \
    worker.cpp \
    simpledownload.cpp \
    applicationinfo.cpp \
    redonserver.cpp \
    analytics.cpp

HEADERS  += mainwindow.h \
    loaderwindow.h \
    systemdata.h \
    downloadlist.h \
    settingsdialog.h \
    configsettings.h \
    urldialog.h \
    videodialog.h \
    lib.h \
    defs.h \
    newdownloader.h \
    confirmdialog.h \
    downloaddialog.h \
    helperclasses.h \
    segmentrow.h \
    filedownloader.h \
    filejoiner.h \
    finisheddialog.h \
    propertydialog.h \
    aboutdialog.h \
    version.h \
    blockmanagementsystem.h \
    masterbuffermanager.h \
    workerspool.h \
    worker.h \
    simpledownload.h \
    applicationinfo.h \
    redonserver.h \
    analytics.h

FORMS    += mainwindow.ui \
    loaderwindow.ui \
    settingsdialog.ui \
    urldialog.ui \
    videodialog.ui \
    confirmdialog.ui \
    downloaddialog.ui \
    finisheddialog.ui \
    propertydialog.ui \
    aboutdialog.ui

RESOURCES += \
    icons.qrc

OTHER_FILES += \
    app_icon.rc

RC_FILE = app_icon.rc

