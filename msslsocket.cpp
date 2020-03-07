#include "msslsocket.h"

MSSLSocket::MSSLSocket()
{
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

int MSSLSocket::connectToHostEncrypted(char *hostname, int port)
{

    // check for previous Error
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
        sprintf(errorString, " Winsock Error: Failed To Establish With Server.");
        WSACleanup();
        return -1;
    }
    return this->beginConnection();

}

int MSSLSocket::beginConnection()
{

    this->sslContext = NULL;
    this->sslHandle = NULL;

    if( this->errorFlag == false ) {

        // Register the error strings for libcrypto & libssl
        SSL_load_error_strings ();
        // Register the available ciphers and digests
        SSL_library_init ();

        // New context saying we are a client, and using SSL 2 or 3
        this->sslContext = SSL_CTX_new (SSLv23_client_method ());
        if( this->sslContext == NULL ) {
            ERR_print_errors_fp (stderr);
        }

        // Create an SSL struct for the connection
        this->sslHandle = SSL_new (this->sslContext);
        if (this->sslHandle == NULL) {
           ERR_print_errors_fp (stderr);
        }


        // Connect the SSL struct to our connection
        if (!SSL_set_fd (this->sslHandle, this->sock )) {
          ERR_print_errors_fp (stderr);
        }

        // Initiate SSL handshake
        if (SSL_connect (this->sslHandle) != 1) {
          ERR_print_errors_fp (stderr);
        }

       return 0;
    } else {

        errorFlag = true;
        sprintf(errorString, " SSL Error: Cannot Establish Secure Connection ");
        WSACleanup();
        return -1;

    }
}

int MSSLSocket::close()
{

    if( this->sock ) {
        closesocket(  this->sock );
    }

    if( this->sslHandle ) {
        SSL_shutdown ( this->sslHandle );
        SSL_free ( this->sslHandle );
    }

    if( this->sslContext ) {
        SSL_CTX_free ( this->sslContext );
    }

    WSACleanup();
    return 0;
}

