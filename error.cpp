//  Copyright © 2011  Vinícius dos Santos Oliveira

/*!
  @warning this file should be included only by responsehandler.cpp
  */

#include "responsehandler.h"

#include <qt-json/json.h>

JsonRPC::Error::Error(ErrorCode code) :
    code(code)
{
    switch (code) {
    case PARSE_ERROR:
        desc = "Invalid JSON was received by the server.";
        break;
    case INVALID_REQUEST:
        desc = "The JSON sent is not a valid Request object.";
        break;
    case METHOD_NOT_FOUND:
        desc = "The method does not exist / is not available.";
        break;
    case INVALID_PARAMS:
        desc = "Invalid method parameter(s).";
        break;
    case INTERNAL_ERROR:
    default:
        this->code = NO_ERROR;
    case NO_ERROR:
        break;
    }
}

JsonRPC::Error::Error(ErrorCode code, QString desc) :
    code(code),
    desc(desc)
{
}

JsonRPC::Error::operator QByteArray() const
{
    return QtJson::Json::serialize(static_cast<QVariantMap>(*this));
}

JsonRPC::Error::operator QVariantMap() const
{
    QVariantMap obj;

    obj.insert("jsonrpc", "2.0");
    {
        QVariantMap errorObj;
        errorObj.insert("code", int(code));
        errorObj.insert("message", desc);

        obj.insert("error", errorObj);
    }

    return obj;
}
