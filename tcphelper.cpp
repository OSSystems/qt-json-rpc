/*
  Copyright © 2011  Vinícius dos Santos Oliveira

  This file is part of Phobos-RPC.

  Phobos-RPC is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#include "tcphelper.h"
#include <QTcpSocket>
#include <QDataStream>

using namespace Phobos;

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
        socket->setParent(this);

        connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));


        peer = new Peer(this);

        connect(peer, SIGNAL(readyRequestMessage(QByteArray)),
                this, SLOT(onReadyMessage(QByteArray)));
        connect(peer, SIGNAL(readyResponseMessage(QByteArray)),
                this, SLOT(onReadyMessage(QByteArray)));

        connect(peer, SIGNAL(readyResponse(QVariant,QVariant)),
                this, SIGNAL(readyResponse(QVariant,QVariant)));
        connect(peer, SIGNAL(requestError(int,QString,QVariant,QVariant)),
                this, SIGNAL(requestError(int,QString,QVariant,QVariant)));
        connect(peer,
                SIGNAL(readyRequest(QSharedPointer<Phobos::ResponseHandler>)),
                this,
                SIGNAL(readyRequest(QSharedPointer<Phobos::ResponseHandler>)));

        this->socket = socket;
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

void TcpHelper::onReadyMessage(const QByteArray &json)
{
    {
        QDataStream stream(socket);
        quint16 size = json.size();
        stream << size;
    }
    socket->write(json);
}

void TcpHelper::onReadyRead()
{
    buffer.append(socket->readAll());

    if (nextMessageSize)
        goto STATE_WAITING_FOR_CONTENT;

    STATE_UNKNOW_SIZE:
    // nextMessageSize is 2 bytes long (quint16)
    if (buffer.size() >= 2) {
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        stream >> nextMessageSize;
        buffer.remove(0, 2);
    } else {
        return;
    }

    STATE_WAITING_FOR_CONTENT:
    if (buffer.size() >= nextMessageSize) {
        peer->handleMessage(buffer.left(nextMessageSize));
        buffer.remove(0, nextMessageSize);

        nextMessageSize = 0;
        goto STATE_UNKNOW_SIZE;
    }
}

void TcpHelper::onDisconnected()
{
    // clear peer data
    delete peer;
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
