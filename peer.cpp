//  Copyright © 2011  Vinícius dos Santos Oliveira

#include "peer.h"
#include "responsehandler.h"

#include <QVariantMap>

#include <qt-json/json.h>

using namespace std;
using namespace JsonRPC;

inline bool isRequestMessage(const QVariantMap &object)
{
    if (object.contains("method"))
        return true;
    else
        return false;
}

inline bool isRequestMessage(const QVariantList &objectList)
{
    if (objectList.size())
        return true;
    else
        return false;
}

inline bool isRequestMessage(const QVariant &object)
{
    switch (object.type()) {
    case QVariant::Map:
        return isRequestMessage(object.toMap());
    case QVariant::List:
        return isRequestMessage(object.toList());
    default:
        return false;
    }
}

inline bool isResponseMessage(const QVariantMap &object)
{
    if (object.contains("result")
            || object.contains("error"))
        return true;
    else
        return false;
}

inline bool isResponseMessage(const QVariantList &objectList)
{
    if (objectList.size())
        return true;
    else
        return false;
}

inline bool isResponseMessage(const QVariant &object)
{
    switch (object.type()) {
    case QVariant::Map:
        return isResponseMessage(object.toMap());
    case QVariant::List:
        return isResponseMessage(object.toList());
    default:
        return false;
    }
}

inline bool isSignalMessage(const QVariantMap &object)
{
    if (object.contains("signal"))
        return true;
    else
        return false;
}

inline bool isSignalMessage(const QVariantList &objectList)
{
    if (objectList.size())
        return true;
    else
        return false;
}

inline bool isSignalMessage(const QVariant &object)
{
    switch (object.type()) {
    case QVariant::Map:
        return isSignalMessage(object.toMap());
    case QVariant::List:
        return isSignalMessage(object.toList());
    default:
        return false;
    }
}

Peer::Peer(QObject *parent) :
    QObject(parent)
{
}

void Peer::handleMessage(const QByteArray &json)
{
    bool ok;
    QVariant object = QtJson::Json::parse(QString::fromUtf8(json), ok);

    if (!ok) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(PARSE_ERROR)));
        return;
    }

    if (isRequestMessage(object))
        handleRequest(object);
    else if (isResponseMessage(object))
        handleResponse(object);
    else if (isSignalMessage(object))
        handleSignal(object);
    else
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
}

void Peer::handleRequest(const QVariant &json)
{
    if (json.type() != QVariant::Map) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
        return;
    }

    QVariantMap object = json.toMap();

    if (!object.contains("method")) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
        return;
    }

    QVariant method = object["method"];
    QSharedPointer<JsonRPC::ResponseHandler> handler(new ResponseHandler(this));

    if (method.type() == QVariant::String) {
        if (!handler->setMethod(method.toString())) {
            emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
            return;
        }
    } else {
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
        return;
    }

    if (object.contains("params")) {
        QVariant params = object["params"];

        if (!handler->setParams(params)) {
            emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
            return;
        }
    }

    if (object.contains("id")) {
        QVariant id = object["id"];

        if (!handler->setId(id)) {
            emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
            return;
        }
    }

    emit readyRequest(handler);
}

void Peer::reply(const QVariant &json)
{
    emit readyResponseMessage(QtJson::Json::serialize(json));
}

bool Peer::call(const QString &method, const QVariant &params, const QVariant &id)
{
    if (method.startsWith("rpc.")
            || (params.type() != QVariant::List
                && params.type() != QVariant::Map
                && !params.isNull())
            || (id.type() != QVariant::String
                && id.type() != QVariant::Int
                && id.type() != QVariant::ULongLong
                && id.type() != QVariant::LongLong
                && id.type() != QVariant::Double
                && !id.isNull()))
        return false;

    QVariantMap object;

    object.insert("jsonrpc", "2.0");

    object.insert("method", method);

    if (!params.isNull())
        object.insert("params", params);

    object.insert("id", id);

    emit readyRequestMessage(QtJson::Json::serialize(object));
    return true;
}

void Peer::emitSignal(const QString &signal, const QVariantList &params)
{
    QVariantMap object;
    object.insert("jsonrpc", "2.0-EXTENSION");
    object.insert("signal", signal);
    object.insert("params", params);

    emit readySignalMessage(QtJson::Json::serialize(object));
}

void Peer::handleResponse(const QVariant &json)
{
    QVariantList objects;
    if (json.type() == QVariant::Map) {
        objects.push_back(json);
    } else if (json.type() == QVariant::List) {
        objects = json.toList();
    } else {
        return;
    }

    Q_FOREACH (const QVariant &object, objects) {
        if (object.type() == QVariant::Map) {
            QVariantMap objectMap = object.toMap();

            if (objectMap.contains("result")) {
                emit readyResponse(objectMap.value("result"),
                                   objectMap.value("id"));
            } else if (objectMap.contains("error")
                       && objectMap.value("error").type() == QVariant::Map) {
                QVariantMap errorObject = objectMap["error"].toMap();

                if (!errorObject.contains("code")
                        || (errorObject.value("code").type() != QVariant::Int
                            && errorObject.value("code").type() != QVariant::ULongLong
                            && errorObject.value("code").type() != QVariant::LongLong)
                        || !errorObject.contains("message")
                        || errorObject.value("message").type() != QVariant::String)
                    continue;

                int code = errorObject["code"].toInt();
                QString message = errorObject["message"].toString();
                QVariant data = errorObject.contains("data") ? errorObject["data"]
                                                             : QVariant();
                QVariant id = objectMap.contains("id") ? objectMap["id"]
                                                       : QVariant();

                emit requestError(code, message, data, id);
            }
        }
    }
}

void Peer::handleSignal(const QVariant &json)
{
    if (json.type() != QVariant::Map) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
        return;
    }

    QVariantMap object = json.toMap();

    if (!object.contains("signal")) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
        return;
    }

    QVariant signal = object["signal"];
    if (signal.type() != QVariant::String) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
        return;
    }

    QVariant params;
    if (object.contains("params")) {
        params = object["params"];
        const QVariant::Type paramsType = params.type();
        if (paramsType != QVariant::List
            && paramsType != QVariant::Map
            && !params.isNull())
        {
            emit readyResponseMessage(static_cast<QByteArray>(Error(INVALID_REQUEST)));
            return;
        }
    }

    emit readySignal(signal.toString(), params);
}
