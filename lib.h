#ifndef LIB_H
#define LIB_H


#include "defs.h"
#include <QtCore>
#include <QtGui>

#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QNetworkReply>


void fileDebug (QString str );

bool isValidUrl(QString url);
QByteArray qStr2Ba(const QString &string);
QString sizeToString(qint64 length);
QString timeToString(qint64 time);
float calculatePercent(qint64 num, qint64 dem);
void QuickSort(int a[], int beg, int end);
QString getNetworkError(QNetworkReply::NetworkError err );

QString getVersionString();

inline int ishex(int x);
int decode(const char *s, char *dec);



#endif // LIB_H
