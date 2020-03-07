#ifndef MTCPSOCKET_H
#define MTCPSOCKET_H

#include <winsock2.h>
#include <stdio.h>


class MTcpSocket {

private:
    WSADATA wsa;
    SOCKET sock;
    sockaddr_in sockAddr;
    bool errorFlag;
    char errorString[200];
    struct hostent *host;

public:
    MTcpSocket();
    int connectToHost (char *hostname, int port);

    inline char *getErrorInfo() {
        return this->errorString;
    }

    inline int write(char *data, int len) {
        return send( this->sock, data, len, 0);
    }

    inline int read(char *buffer,int len ) {
        return recv ( this->sock, buffer, len, 0);
    }

    int close ();

};


#endif // MTCPSOCKET_H
