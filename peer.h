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

#ifndef PHOBOS_PEER_H
#define PHOBOS_PEER_H

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

namespace Phobos {

class ResponseHandler;

/*!
  JSON-RPC 2.0 handler (server and client)
  */
class Peer: public QObject
{
    Q_OBJECT
public:
    /*!
      Constructs an object with parent object \param parent.
      */
    Peer(QObject *parent = NULL);

signals:
    /*!
      Emitted when a new request message is available.
      /param handler is the object that you use to send a response.
      @sa handleMessage
      */
    void readyRequest(QSharedPointer<Phobos::ResponseHandler> handler);
    /*!
      Emitted when the message for your call is available.
      \param json is the message that you need to send to the other
      peer.
      @sa call
      */
    void readyRequestMessage(QByteArray json);

    /*!
      Emitted when the result for your call is available.
      \param result is the result to your call of id \param id.
      @sa handleMessage
      */
    void readyResponse(QVariant result, QVariant id);
    /*!
      Emitted when the message for your response is available.
      \param json is the message that you need to send to the other
      peer.
      @sa ResponseHandler::response ResponseHandler::error
      */
    void readyResponseMessage(QByteArray json);

    /*!
      Emitted when a error response message is received.
      \param code is the error code (see the ErrorCode enum),
      \param message is a human-readable string, and data is
      custom data sent by the server.
      */
    void requestError(int code, QString message, QVariant data, QVariant id);

public slots:
    /*!
      It parses \param json and emit the signals to correctly handle the
      message.
      Use this method every time that you have a new message to handle.
      */
    void handleMessage(const QByteArray &json);
    /*!
      It handles a request message.
      @sa handleMessage
      */
    void handleRequest(const QVariant &json);
    /*!
      It handles a response message
      @sa handleMessage
      */
    void handleResponse(const QVariant &json);

    /*!
      Use this method to emit the readyResponseMessage signal.
      You probably don't want to use this.
      It's used by the ResponseHandler class.
      */
    void reply(const QVariant &json);

    /*!
      Prepares a request message.
      @return true if \param method, \param params and \param id are valid,
      according JSON-RPC 2.0 spec.
      */
    bool call(const QString &method, const QVariant &params, const QVariant &id);
};

} // namespace Phobos

#endif // PHOBOS_PEER_H
