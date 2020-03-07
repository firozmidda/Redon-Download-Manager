#ifndef REDONSERVER_H
#define REDONSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>


class RedonServer : public QObject
{
    Q_OBJECT
public:
    explicit RedonServer(QObject *parent = 0);
      ~RedonServer();
    inline int getError () {  return this->errorFlag; }


    QTcpServer *server;
    QTcpSocket *client;
    int errorFlag ;

signals:
    void gotRequest(QString str);

public slots:

public slots:
  void acceptConnection();
  void startRead();
  void serverError(QAbstractSocket::SocketError err);
  void disconnect ();

};

#endif // REDONSERVER_H
