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

#ifndef PHOBOS_TCPHELPER_H
#define PHOBOS_TCPHELPER_H

#include "peer.h"

class QTcpSocket;

namespace Phobos {

/*! TcpHelper is a helper class to use JSON-RPC over tpc sockets.
  It uses the core classes of Phobos to implement this.
  The protocol is:

  [message size][JSON-RPC message]

  [message size] is a 16-bit unsigned integer, serialized by QDataStream

  Using this class you only need to care about handle the rpc requests,
  not the communication layer.
  @warning using this protocol, the maximum size for each message is 65535
  bytes.
  */
class PHOBOSRPC_EXPORT TcpHelper : public QObject
{
    Q_OBJECT
public:
    explicit TcpHelper(QObject *parent = 0);

    /*! Sets the socket used be in the communication.
      \param socket must be in connected state.
      The TcpHelper takes parentship.
      If you pass a NULL value, then TcpHelper will just throw the old
      socket.
      @return true in success (socket connected)
      */
    bool setSocket(QTcpSocket *socket);

signals:
    /*!
      Emitted when the result for your call is available.
      \param result is the result to your call of id \param id.
      @sa handleMessage
      */
    void readyResponse(QVariant result, QVariant id);
    /*!
      Emitted when a error response message is received.
      \param code is the error code (see the ErrorCode enum),
      \param message is a human-readable string, and data is
      custom data sent by the server.
      */
    void requestError(int code, QString message, QVariant data, QVariant id);
    /*!
      Emitted when a new request message is available.
      /param handler is the object that you use to send a response.
      @sa handleMessage
      */
    void readyRequest(QSharedPointer<Phobos::ResponseHandler> handler);

    /*!
      Emitted when the socket has been disconnected.
      */
    void disconnected();

public slots:
    /*!
      Prepares a request message.
      @return true if \param method, \param params and \param id are valid,
      according JSON-RPC 2.0 spec.
      */
    bool call(const QString &method, const QVariant &params, const QVariant &id);

private slots:
    void onReadyMessage(const QByteArray &json);
    void onReadyRead();
    void onDisconnected();

private:
    Peer *peer;

    QTcpSocket *socket;
    QByteArray buffer;
    quint16 nextMessageSize;
};

} // namespace Phobos

#endif // PHOBOS_TCPHELPER_H
