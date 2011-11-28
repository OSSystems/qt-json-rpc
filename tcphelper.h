// Copyright © 2011  Vinícius dos Santos Oliveira

#ifndef PHOBOS_TCPHELPER_H
#define PHOBOS_TCPHELPER_H

#include "peer.h"

class QTcpSocket;

namespace JsonRPC {

/*! TcpHelper is a helper class to use JSON-RPC over tpc sockets.
  It uses the core classes of JsonRPC to implement this.
  The protocol is:

  [message size][JSON-RPC message]

  [message size] is a 16-bit unsigned integer, serialized by QDataStream

  Using this class you only need to care about handle the rpc requests,
  not the communication layer.
  @warning using this protocol, the maximum size for each message is 65535
  bytes.
  */
class TcpHelper : public QObject
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
    void readyRequest(QSharedPointer<JsonRPC::ResponseHandler> handler);

    /*!
      Emitted when a new signal message is available.
      /param signal is the signal name and params is the signal params.
      @sa handleMessage
      */
    void readySignal(QString signal, QVariant params);

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

    /*!
      Send a signal
      THIS IS A EXTENSION TO THE JSON-RPC PROTOCOL
      */
    void emitSignal(const QString &signal, const QVariantList &params);

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

} // namespace JsonRPC

#endif // PHOBOS_TCPHELPER_H
