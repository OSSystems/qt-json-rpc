//  Copyright © 2011  Vinícius dos Santos Oliveira

#include "tcphelper.h"
#include <QTcpSocket>
#include <QDataStream>

using namespace JsonRPC;

TcpHelper::TcpHelper(QObject *parent) :
    QObject(parent),
    peer(NULL),
    socket(NULL),
    nextMessageSize(0)
{
}

bool TcpHelper::setSocket(QTcpSocket *socket)
{
    if (this->socket)
        onDisconnected();

    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        peer = new Peer(this);

        connect(peer, SIGNAL(readyRequestMessage(QByteArray)),
                this, SLOT(onReadyMessage(QByteArray)));
        connect(peer, SIGNAL(readyResponseMessage(QByteArray)),
                this, SLOT(onReadyMessage(QByteArray)));
        connect(peer, SIGNAL(readySignalMessage(QByteArray)),
                this, SLOT(onReadyMessage(QByteArray)));

        connect(peer, SIGNAL(readyResponse(QVariant,QVariant)),
                this, SIGNAL(readyResponse(QVariant,QVariant)));
        connect(peer, SIGNAL(requestError(int,QString,QVariant,QVariant)),
                this, SIGNAL(requestError(int,QString,QVariant,QVariant)));
        connect(peer,
                SIGNAL(readyRequest(QSharedPointer<JsonRPC::ResponseHandler>)),
                this,
                SIGNAL(readyRequest(QSharedPointer<JsonRPC::ResponseHandler>)));
        connect(peer, SIGNAL(readySignal(QString,QVariant)),
                this, SIGNAL(readySignal(QString,QVariant)));

        this->socket = socket;

        socket->setParent(this);

        connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

        return true;
    } else {
        return false;
    }
}

bool TcpHelper::call(const QString &method, const QVariant &params, const QVariant &id)
{
    if (peer)
        return peer->call(method, params, id);
    else
        return false;
}

void TcpHelper::emitSignal(const QString &signal, const QVariantList &params)
{
    if (peer)
        peer->emitSignal(signal, params);
}

void TcpHelper::onReadyMessage(const QByteArray &json)
{
    QDataStream stream(socket);
    stream.setVersion(QDataStream::Qt_4_6);
    quint16 size = json.size();
    stream << size;
    stream << json;
}

void TcpHelper::onReadyRead()
{
    QDataStream stream(socket);
    stream.setVersion(QDataStream::Qt_4_6);

    if (nextMessageSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        stream >> nextMessageSize;
    }

    if (socket->bytesAvailable() < nextMessageSize)
        return;

    QByteArray data;
    stream >> data;
    buffer.append(data);

    if (buffer.size() == nextMessageSize) {
        peer->handleMessage(buffer);
        buffer.clear();
        nextMessageSize = 0;
    }
}

void TcpHelper::onDisconnected()
{
    // clear peer data
    peer->deleteLater();
    peer = NULL;

    // clear buffer data
    buffer.clear();
    nextMessageSize = 0;

    // clear socket data
    socket->disconnect();
    socket->deleteLater();
    socket = NULL;

    emit disconnected();
}
