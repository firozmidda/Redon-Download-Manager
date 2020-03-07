#ifndef MSSLSOCKET_H
#define MSSLSOCKET_H

#include <sys/types.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>




#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>





class MSSLSocket
{


private:
    WSADATA wsa;
    SOCKET sock;
    SSL *sslHandle;
    SSL_CTX *sslContext;

    sockaddr_in sockAddr;
    bool errorFlag;
    char errorString[200];
    struct hostent *host;

public:
    MSSLSocket();
    int connectToHostEncrypted (char *hostname, int port);
    int beginConnection();


    inline char *getErrorInfo() {
        return this->errorString;
    }

    inline int write(char *data, int len) {
        return  SSL_write( this->sslHandle, data, len);
    }

    inline int read(char *buffer,int len ) {
        return SSL_read ( this->sslHandle, buffer, len );
    }

    int close ();
};

#endif // MSSLSOCKET_H
