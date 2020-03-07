#include "redonserver.h"


RedonServer::RedonServer(QObject *parent) :
    QObject(parent)
{

    int port = 47777;

    this->errorFlag = false;
    QTcpSocket socket;
    socket.connectToHost("127.0.0.1", port);

   // we need to wait...
    if( socket.waitForConnected(5000))   {
       qDebug () << " Port "  << port  << " Is Already in Use ";
       this->errorFlag = true;
    } else {

        qDebug () << " Port " << port << " is free ";
        this->server = new QTcpServer( this );

        connect( this->server, SIGNAL( acceptError(QAbstractSocket::SocketError)), this, SLOT( serverError(QAbstractSocket::SocketError)));
        connect( this->server, SIGNAL( newConnection()), this, SLOT( acceptConnection()));

        if( !this->server->listen( QHostAddress::Any, port)) {
            this->errorFlag = true;
            qDebug () << " Cannot Listen To port " << port;
        } else {
            qDebug () << " Finally Server Started ";
        }

    }


    if( this->errorFlag ) {

        qDebug () << " Error Occured ";

        // emit error // cannot start the sever
    }


}

RedonServer::~RedonServer()
{
    this->server->close();

}



void RedonServer::acceptConnection()
{
    qDebug () << " New Request Arrived";
    client = this->server->nextPendingConnection();
    connect ( client, SIGNAL(readyRead()), this, SLOT(startRead()));
    connect ( client, SIGNAL(disconnected()), this , SLOT( disconnect()) );

}

void RedonServer::startRead()
{

    qDebug () << " Got Request  ";
    char buffer[2048] = {0};
    char lineData[1024] = {0};

    client->read(buffer, client->bytesAvailable());

    int i = 0, j = 0;
    while ( buffer[i] != ' ') {
        i++;
    }
    i++;

    while ( buffer[i] != ' ' ) {
        lineData [j ++] = buffer[i];
        i++;
    }

    // cout >> buffer >> endl;
    client->write("HTTP/1.1 200 OK\r\n\r\n asdasdasd \r\n\r\n ");
    client->close();

    emit gotRequest( QString(lineData) );

}

void RedonServer::serverError(QAbstractSocket::SocketError err)
{
    qDebug () << " Error Occured " << err;
}

void RedonServer::disconnect()
{
    qDebug () << " Socket Disconnect ";
}
