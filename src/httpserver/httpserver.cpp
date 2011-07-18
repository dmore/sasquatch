#include "httpserver.h"

#include "client.h"

HttpServer::HttpServer(quint16 port, QObject *parent) :
    QTcpServer(parent)
{
    listen(QHostAddress::Any, port);
    qDebug() << "Streaming server listening" << serverAddress().toString() << "on" << serverPort();
}

void HttpServer::incomingConnection(int socket)
{
    qDebug() << "New Connection";

    QThread *thread = new QThread(this);
    Client *client = new Client(socket, this);
    client->moveToThread(thread);
    connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
    connect(client, SIGNAL(disconnected()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void HttpServer::setBase(const QString &b)
{
    if (b == m_base)
        return;

    m_base = b;
    emit baseChanged();
}

QString HttpServer::base() const
{
    return m_base;
}
