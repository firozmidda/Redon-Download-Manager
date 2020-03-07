
#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QUrl>
#include <QHash>
#include "httpresponse.h"




class HttpRequest
{
    QUrl url;
    QString resourcePath;
    QHash<QString, QString> requestHeaderHash;
    HttpResponse *res;
public:
    HttpRequest(QUrl url);
    void setRawRequest(QString key, QString value);
    QString getHeaderRequestStr ();
    HttpResponse *get();
    HttpResponse *head();



};

#endif // HTTPREQUEST_H
