#include "mtcpsocket.h"


MTcpSocket::MTcpSocket() {

    errorFlag = false;

    if( WSAStartup( MAKEWORD (2, 2), &wsa) != 0) {
        errorFlag = true;
        sprintf(errorString, " Winsock Error: Winsock Initialization Failed. Error Code %d ", WSAGetLastError());
        WSACleanup();
        return ;
    }


    // create Socket
    this->sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( this->sock == INVALID_SOCKET){
        errorFlag = true;
        sprintf(errorString, " Winsock Error: Socket Creation Failed. Error Code %d ", WSAGetLastError());

        WSACleanup();
        return ;
    }
}

int MTcpSocket::connectToHost(char *hostname, int port)
{
    // check previous Error
    if( this->errorFlag ) {
        return -1;
    }

    // Resolve IP address for hostname
    if((host = gethostbyname(hostname)) == NULL) {
        errorFlag = true;
        sprintf(errorString, " Winsock Error: Failed To Resolve hostname.");
        WSACleanup();
        return -1;
    }


    /// setup our socket address structure
    this->sockAddr.sin_port =  htons( port ) ;
    this->sockAddr.sin_family = AF_INET;
    this->sockAddr.sin_addr.s_addr = *((unsigned long*) host->h_addr);


    // Attempt to connect to server
    if( connect (this->sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0 ) {
        errorFlag = true;
        sprintf(errorString, " Winsock Error: Failed TO Establish With Server.");
        WSACleanup();
        return -1;
    }

}

int MTcpSocket::close()
{
    // shut down socket
    shutdown(this->sock, SD_SEND);

    // Close our socket entirely
    closesocket( this->sock);

    // cleanup Winsock
    WSACleanup();
    return 0;

}
