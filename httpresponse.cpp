#include "httpresponse.h"

#include <QDebug>

HttpResponse::HttpResponse(QUrl url, QString data)
{
    this->url = url;
    this->sslFlag = false;

    if( this->url.scheme() == "https") {
        this->sslFlag = true;
    }

    if( this->sslFlag ) {
        this->sslSocket = new MSSLSocket();
    } else {
        this->tcpSocket = new MTcpSocket();
    }

    this->makeRequest( url, data);
    this->readHeader ();


}

void HttpResponse::makeRequest( QUrl url, QString data)
{
    if( this->sslFlag ) {
        int port = url.port( 443);
        int result = this->sslSocket->connectToHostEncrypted( url.host().toLocal8Bit().data() , port);
        this->sslSocket->write( data.toLocal8Bit().data(), data.length());
    } else {
        int port = url.port( 80);
        this->tcpSocket->connectToHost(  url.host().toLocal8Bit().data(), port);
        this->tcpSocket->write( data.toLocal8Bit().data(), data.length());
    }

}

void HttpResponse::readHeader()
{
    this->hasData = false;
    this->bufferSize = 0;

    char *headerBuffer = new char [ 1024* 10 ];
    char tmpBuffer[1025];
    int pos = 0;

    int headerStart = 0, headerEnd = 0;
    int dataStart = 0, dataEnd = 0;

    memset( tmpBuffer, 0, 1025);
    int readSize = 0;


    while ( (readSize = this->read(tmpBuffer, 1024)) > 0 ) {


        char *str = strstr ( tmpBuffer, "\r\n\r\n");
        headerStart = 0;
        headerEnd = ( str - tmpBuffer );

        int size = ( headerEnd > 0 )  ? headerEnd : readSize ;

        memcpy( headerBuffer , tmpBuffer, size );

        pos += size;

        if( headerEnd > 0 ) {
            dataStart = headerEnd + 4;
            dataEnd = readSize;
            break;
        }

        break;


    }

    headerBuffer[pos + 1] = '\0';

    if( dataEnd > 0 ) {

        this->hasData = true;
        int size = dataEnd - dataStart;
        memcpy( buffer, tmpBuffer + dataStart, size);
        buffer[ size + 1] = '\0';
        bufferSize = size;

    }

    parseHeader ( headerBuffer ) ;
    delete headerBuffer;

}

void HttpResponse::parseHeader(char *headerBuffer)
{
    QStringList lines = QString( headerBuffer).split( CRLF );
    this->parseResponseCode( lines [0]);

   // qDebug () << " lines length = " << lines.length();

    for( int i = 1 ; i < lines.length(); i ++ ) {
        QStringList tmp = lines[i].split(": ");
        this->responseData.insert( tmp[0].trimmed(), tmp[1].trimmed() );

    }

}

void HttpResponse::parseResponseCode(QString data)
{

    QStringList tmp = data.split(' ');
    QString version = tmp[0].split('/')[1];
    this->httpVersion = version.toFloat();
    this->responseCode = tmp[1].toInt();
    this->responseMessage = tmp[2];


}

int HttpResponse::read(char *buffer, int len)
{
    if( this->sslFlag ) {
        return this->sslSocket->read(buffer, len);
    } else {
        return this->tcpSocket->read( buffer, len);
    }

}

int HttpResponse::readData(char *buf, int len)
{
    if( this->hasData ) {
        this->hasData = false;
        for( int i = 0; i< bufferSize; i ++ ) {
            buf[i] = buffer[i];
        }
        return bufferSize;

    } else {
        return this->read( buf, len);
    }


}



int HttpResponse::close()
{
    if( this->sslFlag ) {
        this->sslSocket->close();
    } else {
        this->tcpSocket->close();
    }

}


