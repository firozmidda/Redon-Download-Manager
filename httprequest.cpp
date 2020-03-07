#include "httprequest.h"

HttpRequest::HttpRequest(QUrl url)
{

    // correcting the url
    if( url.scheme() == "") {
        url.setScheme("http");
    }

    this->url = url;
    this->resourcePath = this->url.path();
    if( this->url.hasQuery() ) {
        resourcePath += "?";
        resourcePath += url.query();
    }

    if( resourcePath == "") {
        resourcePath = "/";
    }


    this->setRawRequest("Host", url.host());
    this->setRawRequest("User-Agent", "Mozilla/5.0 (Windows NT 6.1; rv:25.0) Gecko/20100101 Firefox/25.0");
    this->setRawRequest("Accept",  "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
    this->setRawRequest("Accept-Language", "en-US,en;q=0.5");
    //this->setRawRequest("Accept-Encoding", "gzip, deflate");
    this->setRawRequest("Connection", "keep-alive");


}

void  HttpRequest::setRawRequest(QString key, QString value) {
    this->requestHeaderHash[key] = value;
}

QString HttpRequest::getHeaderRequestStr()
{
    QHash<QString, QString>::iterator i;
    QString requestStr;
    for( i = this->requestHeaderHash.begin(); i != this->requestHeaderHash.end();  i++) {
        requestStr += i.key() +": " + i.value()  + CRLF;
    }
    return requestStr;
}

HttpResponse* HttpRequest::get() {
    QString requestStr = "GET " + resourcePath + " HTTP/1.1" + CRLF + this->getHeaderRequestStr() + CRLF;
    this->res = new HttpResponse( this->url, requestStr);
    return this->res;

}

HttpResponse* HttpRequest::head() {
    QString requestStr =  "HEAD " + resourcePath + " HTTP/1.1" + CRLF + this->getHeaderRequestStr() + CRLF;
    this->res = new HttpResponse( this->url, requestStr);
    return this->res;
}
