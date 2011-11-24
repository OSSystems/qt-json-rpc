//  Copyright © 2011  Vinícius dos Santos Oliveira

#include "responsehandler.h"
#include "error.h"
#include "peer.h"

#include <QVariantMap>

using namespace JsonRPC;

ResponseHandler::ResponseHandler(Peer *peer) :
    peer(peer),
    m_hasId(false)
{
}

QString ResponseHandler::method() const
{
    return m_method;
}

bool ResponseHandler::setMethod(const QString &method)
{
    if (method.startsWith("rpc."))
        return false;

    m_method = method;
    return true;
}

bool ResponseHandler::hasParams() const
{
    if (!m_params.isNull()) {
        if (m_params.type() == QVariant::List) {
            return !m_params.toList().isEmpty();
        } else {
            return !m_params.toMap().isEmpty();
        }
    }
    return false;
}

QVariant ResponseHandler::params() const
{
    return m_params;
}

bool ResponseHandler::setParams(const QVariant &params)
{
    const QVariant::Type paramsType = params.type();
    if (paramsType != QVariant::List
            && paramsType != QVariant::Map
            && !params.isNull())
        return false;

    m_params = params;
    return true;
}

void ResponseHandler::resetParams()
{
    m_params = QVariant();
}

bool ResponseHandler::hasId() const
{
    return m_hasId;
}

QVariant ResponseHandler::id() const
{
    return m_id;
}

bool ResponseHandler::setId(const QVariant &id)
{
    const QVariant::Type idType = id.type();
    if (idType != QVariant::String
            && idType != QVariant::Int
            && idType != QVariant::ULongLong
            && idType != QVariant::LongLong
            && idType != QVariant::Double
            && !id.isNull())
        return false;

    m_id = id;
    m_hasId = true;
    return true;
}

void ResponseHandler::resetId()
{
    m_hasId = false;
}

bool ResponseHandler::isNull() const
{
    return !static_cast<bool>(peer);
}

void ResponseHandler::response(const QVariant &result)
{
    if (!m_hasId)
        peer = NULL;

    if (!peer)
        return;

    QVariantMap response;

    response.insert("jsonrpc", "2.0");
    response.insert("result", result);
    response.insert("id", m_id);

    peer->reply(response);

    // doing this will avoid more than one response
    // per request
    peer = NULL;
}

void ResponseHandler::error(const JsonRPC::Error &error)
{
    if (!m_hasId)
        peer = NULL;

    if (!peer)
        return;

    QVariantMap response = static_cast<QVariantMap>(error);

    response.insert("id", m_id);

    peer->reply(response);

    // doing this will avoid more than one response
    // per request
    peer = NULL;
}
