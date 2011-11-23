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
/*!
  @warning this file should be included only by responsehandler.cpp
  */

#include <qjson/serializer.h>

Phobos::Error::Error(ErrorCode code) :
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

Phobos::Error::Error(ErrorCode code, QString desc) :
    code(code),
    desc(desc)
{
}

Phobos::Error::operator QByteArray() const
{
    QJson::Serializer serializer;
    return serializer.serialize(static_cast<QVariantMap>(*this));
}

Phobos::Error::operator QVariantMap() const
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
