//  Copyright © 2011  Vinícius dos Santos Oliveira

#ifndef PHOBOS_PEER_H
#define PHOBOS_PEER_H

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

namespace JsonRPC {

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
    void readyRequest(QSharedPointer<JsonRPC::ResponseHandler> handler);
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
      Emitted when a new signal message is available.
      /param signal is the signal name and params is the signal params.
      @sa handleMessage
      */
    void readySignal(QString signal, QVariant params);

    /*!
      Emitted when the message for your signal is available.
      \param json is the signal that you need to send to the other
      peer.
      @sa emitSignal
      */
    void readySignalMessage(QByteArray json);

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
      It handles a signal message
      @sa handleMessage
      */
    void handleSignal(const QVariant &json);

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

    /*!
      Send a signal
      THIS IS A EXTENSION TO THE JSON-RPC PROTOCOL
      */
    void emitSignal(const QString &signal, const QVariantList &params);
};

} // namespace JsonRPC

#endif // PHOBOS_PEER_H
