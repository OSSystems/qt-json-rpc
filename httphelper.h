//  Copyright © 2011  Vinícius dos Santos Oliveira

#ifndef PHOBOS_HTTPHELPER_H
#define PHOBOS_HTTPHELPER_H

#include "peer.h"
#include <QUrl>
#include <QNetworkReply>

class QNetworkAccessManager;

namespace JsonRPC {

class HttpHelper : public QObject
{
    Q_OBJECT
public:
    explicit HttpHelper(QObject *parent = 0);

    /*!
      @return the current url used in requests.
      */
    QUrl url() const;
    /*! Sets the url to be used in the next requests.
      \param url is the new url.
      */
    void setUrl(const QUrl &url);

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
      Emitted when the QNetworkReply object detects an error in processing.
      */
    void error(QNetworkReply::NetworkError code);

public slots:
    /*!
      Prepares a request message.
      @return true if \param method, \param params and \param id are valid,
      according JSON-RPC 2.0 spec.
      */
    bool call(const QString &method, const QVariant &params, const QVariant &id);

private slots:
    void onReadyRequestMessage(const QByteArray &json);
    void replyFinished(QNetworkReply *reply);

private:
    Peer *peer;

    QNetworkAccessManager *httpClient;
    QUrl m_url;
};

} // namespace JsonRPC

#endif // PHOBOS_HTTPHELPER_H
