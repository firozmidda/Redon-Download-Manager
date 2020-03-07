#include "lib.h"

bool isValidUrl(QString url) {

    QRegExp validUrlRegex ("^(http|https)://[a-z0-9]+([-.]{1}[a-z0-9]+)*.[a-z]{2,5}(([0-9]{1,5})?/?.*)$");
    QRegExpValidator validator(validUrlRegex);
    int index = 0;
    return (validator.validate(url, index) == QValidator::Acceptable ) ? true: false;

}

void fileDebug (QString str ) {

    QFile tmpFile ("I:\\REDON\\SRC\\build-Redon-Static-Release\\tmpFile.txt");
    tmpFile.open(QIODevice::Append | QIODevice::WriteOnly );
    QTextStream tmpStream ( &tmpFile );
    tmpStream << str;
    tmpFile.flush();
    tmpFile.close();

}


QByteArray qStr2Ba(const QString &string) {

    return string.toUtf8();
}


QString sizeToString(qint64 length) {

    QString sizeArr[] = {" Bytes", " KB" , " MB", " GB", " TB" };

    int index = 0;
    float size = length;

    while(size > 1024) {
        size = size / 1024;
        index ++;
    }

    return QString::number(size, 'f', 3) + sizeArr[index];

}

QString timeToString(qint64 time) {

    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    QString result = "";
    if( time <=0 ) {
        result = "unknown";
    } else {

        seconds = time % 60;
        minutes = (time / 60) % 60;
        hours = (time / 3600 );

        if( hours > 0 ) {
            result += QString::number(hours)+ " hr ";
            result += QString::number(minutes)+ " min ";
        } else if( minutes > 0 ) {
            result += QString::number(minutes)+ " min ";
            result += QString::number(seconds)+ " sec ";
        } else  if( seconds > 0 ) {
            result += QString::number(seconds)+ " sec ";
        }

    }



    return result;

}


float calculatePercent(qint64 num, qint64 dem) {
    float result = 0;
    if( dem > 0 ) {
        result = float(((float) num * 100 ) / (float) dem);
    }
    return result;
}

int Partition(int a[], int beg, int end)          //Function to Find Pivot Point
{
    int p = beg, pivot = a[beg], loc;

    for(loc = beg+1; loc <= end; loc++)   {

        if( pivot > a[loc] )  {

            a[p] = a[loc];
            a[loc] = a[p+1];
            a[p+1] = pivot;

            p = p+1;
        }
    }
    return p;
}



void QuickSort(int a[], int beg, int end)
{
    if( beg < end)
    {
        int p = Partition(a, beg, end);                       //Calling Procedure to Find Pivot
        QuickSort(a, beg, p-1);                             //Calls Itself (Recursion)
        QuickSort(a, p+1, end);			              //Calls Itself (Recursion)
    }
}


QString getNetworkError(QNetworkReply::NetworkError err)
{
    QString message = "";
    switch ( err) {

        case QNetworkReply::ConnectionRefusedError:	message = 	"The remote server refused the connection (the server is not accepting requests)"; break;
        case QNetworkReply::RemoteHostClosedError:   message = "The remote server closed the connection prematurely, before the entire reply was received and processed"; break;
        case QNetworkReply::HostNotFoundError:	 message =	"The remote host name was not found (invalid hostname)"; break;
        case QNetworkReply::TimeoutError:	 message = "The connection to the remote server timed out"; break;
        case QNetworkReply::OperationCanceledError:	 message = "The operation was canceled via calls to abort() or close() before it was finished."; break;
        case QNetworkReply::SslHandshakeFailedError:	 message = 	"The SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted."; break;
        case QNetworkReply::TemporaryNetworkFailureError:	 message = "The connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established."; break;
        case QNetworkReply::NetworkSessionFailedError:	 message = 	"The connection was broken due to disconnection from the network or failure to start the network."; break;
        case QNetworkReply::BackgroundRequestNotAllowedError:  message = "The background request is not currently allowed due to platform policy."; break;
        case QNetworkReply::ProxyConnectionRefusedError:	 message = "The connection to the proxy server was refused (the proxy server is not accepting requests)"; break;
        case QNetworkReply::ProxyConnectionClosedError:  message =	"The proxy server closed the connection prematurely, before the entire reply was received and processed"; break;
        case QNetworkReply::ProxyNotFoundError:	 message =	"The proxy host name was not found (invalid proxy hostname)"; break;
        case QNetworkReply::ProxyTimeoutError:  message =	"The connection to the proxy timed out or the proxy did not reply in time to the request sent"; break;
        case QNetworkReply::ProxyAuthenticationRequiredError:  message =	"The proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)"; break;
        case QNetworkReply::ContentAccessDenied:  message =		"The access to the remote content was denied (similar to HTTP error 401)"; break;
        case QNetworkReply::ContentOperationNotPermittedError:  message = 		"The operation requested on the remote content is not permitted"; break;
        case QNetworkReply::ContentNotFoundError:  message =		"The remote content was not found at the server (similar to HTTP error 404)"; break;
        case QNetworkReply::AuthenticationRequiredError:  message =		"The remote server requires authentication to serve the content but the credentials provided were not accepted (if any)"; break;
        case QNetworkReply::ContentReSendError:  message =	"The request needed to be sent again, but this failed for example because the upload data could not be read a second time."; break;
        case QNetworkReply::ProtocolUnknownError:  message =	"The Network Access API cannot honor the request because the protocol is not known"; break;
        case QNetworkReply::ProtocolInvalidOperationError:  message =	"The requested operation is invalid for this protocol"; break;
        case QNetworkReply::UnknownNetworkError:  message =	"An unknown network-related error was detected"; break;
        case QNetworkReply::UnknownProxyError:  message =		"An unknown proxy-related error was detected"; break;
        case QNetworkReply::UnknownContentError:  message =		"An unknown error related to the remote content was detected"; break;
        case QNetworkReply::ProtocolFailure:  message = " Network Protocol failure "; break;

    }
   return  message;
}


QString getVersionString()
{
    QString version = APPLICATION_VERSION;
    return version;
}





inline int ishex(int x)
{
    return	(x >= '0' && x <= '9')	||
        (x >= 'a' && x <= 'f')	||
        (x >= 'A' && x <= 'F');
}

int decode(const char *s, char *dec)
{
    char *o;
    const char *end = s + strlen(s);
    int c;

    for (o = dec; s <= end; o++) {
        c = *s++;
        if (c == '+') c = ' ';
        else if (c == '%' && (	!ishex(*s++)	||
                    !ishex(*s++)	||
                    !sscanf(s - 2, "%2x", &c)))
            return -1;

        if (dec) *o = c;
    }

    return o - dec;
}



