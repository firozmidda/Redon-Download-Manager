#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QUrl>
#include <QMap>
#include <QtCore>
#include "mtcpsocket.h"
#include "msslsocket.h"




#define   CRLF  "\r\n"
#define TMP_BUFFER_SIZE 1024

class HttpResponse
{

    QUrl url;
    MTcpSocket *tcpSocket;
    MSSLSocket *sslSocket;



    bool sslFlag;
    char buffer[ TMP_BUFFER_SIZE ];
    bool hasData;
    int bufferSize;


    // response Data
    QMap <QString, QString> responseData;
    int responseCode;
    float httpVersion;
    QString responseMessage;


public:
    HttpResponse(QUrl url, QString data);
    void makeRequest(QUrl url, QString data);
    void readHeader();
    void parseHeader ( char * headerBuffer );
    void parseResponseCode (QString data );
    int read(char * buffer, int len);
    int readData (char * buf, int len );
    int close ();
};

#endif // HTTPRESPONSE_H
