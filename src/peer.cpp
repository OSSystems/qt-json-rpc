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

#include "peer.h"
#include <QVariantMap>
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include "responsehandler.h"

using namespace std;
using namespace Phobos;

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

Peer::Peer(QObject *parent) :
    QObject(parent)
{
}

void Peer::handleMessage(const QByteArray &json)
{
    QJson::Parser parser;

    bool ok;
    QVariant object = parser.parse(json, &ok);

    if (!ok) {
        emit readyResponseMessage(static_cast<QByteArray>(Error(PARSE_ERROR)));
        return;
    }

    if (isRequestMessage(object))
        handleRequest(object);
    else if (isResponseMessage(object))
        handleResponse(object);
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
    QSharedPointer<Phobos::ResponseHandler> handler(new ResponseHandler(this));

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
    QJson::Serializer serializer;
    emit readyResponseMessage(serializer.serialize(json));
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

    QJson::Serializer serializer;
    emit readyRequestMessage(serializer.serialize(object));
    return true;
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
