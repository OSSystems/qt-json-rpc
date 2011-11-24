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

#ifndef PHOBOS_RESPONSEHANDLER_H
#define PHOBOS_RESPONSEHANDLER_H

#include <QVariant>
#include <QPointer>

namespace Phobos {

class Peer;

enum ErrorCode
{
    NO_ERROR,
    PARSE_ERROR      = -32700,
    INVALID_REQUEST  = -32600,
    METHOD_NOT_FOUND = -32601,
    INVALID_PARAMS   = -32602,
    INTERNAL_ERROR   = -32603
};

struct Error
{
    /*!
      @brief Constructs a Error object with \param code error code and
      \param desc error message.
      */
    Error(ErrorCode code, QString desc);
    /*! Constructs a Error object with \param code error code.
      It'll try to set the error message automatically for standard
      error codes. If you pass a non-standard error code,
      ErrorCode::NO_ERROR will be used.
      */
    Error(ErrorCode code = NO_ERROR);
    Error(const Error &);

    /*! Generates the JSON object.
      */
    operator QByteArray() const;
    /*! Generates the QVariantMap object.
      The generated QVariantMap can be parsed into a JSON Object.
      */
    operator QVariantMap() const;

    /*! Error code.
      */
    ErrorCode code;
    /*! Error message.
      */
    QString desc;
};

class ResponseHandler
{
public:
    /*!
      Creates a ResponseHandler that will use \param peer
      when responding some message.
      */
    explicit ResponseHandler(Peer *peer = 0);

    /*! method getter.
      @return a string containing the method name
      */
    QString method() const;
    /*! Sets the method.
      \param method can be any valid name according
      the json-rpc 2.0 spec.
      e.g.: not starting with "rpc."
      @return true if is a valid method
      */
    bool setMethod(const QString &method);

    /*!
      @return true if params map/array exists
      @sa resetParams
      */
    bool hasParams() const;
    /*! params getter
      @return the params object (a QVariantMap or a QVariantList),
      or a null QVariant if params object doesn't exist.
      @sa hasParams
      @sa setParams
      */
    QVariant params() const;
    /*! params setter
      \param params can be any valid params object according
      the json-rpc 2.0 spec (QVariantMap and QVariantList).
      @return true if is a valid params object.
      @sa params
      */
    bool setParams(const QVariant &params);
    /*! Remove the params object.
      @sa hasParams
      @sa setParams
      */
    void resetParams();

    /*!
      @return true if the response has an id.
      @sa resetId
      */
    bool hasId() const;
    /*!
      If id doesn't exists, then a response message won't be
      generated when you use the response or error methods.
      @return the id, or a null QVariant if id doesn't exist.
      @sa hasId
      @sa setId
      */
    QVariant id() const;
    /*! Validates and sets the id.
      This id will be used in the JSON-RPC response message object.
      @return true if \param id is a valid id.
      @sa id
      */
    bool setId(const QVariant &id);
    /*! Removes any id associated with the response object.
      @sa hasId
      @sa setId
      */
    void resetId();

    /*! A ResponseHandler object is null if you can't send a message.
      This happens when you already sent a message (either using response
      or error methods) or when you constructed the ResponseHandler with
      no peer
      @return false if you can send a reply to the peer object.
      */
    bool isNull() const;
    /*! Sends the response object to the peer object, if it still exists.
      Use this method when you wants send the response.
      @warning use this method when the object is in null state won't do
      anything
      @sa isNull
      */
    void response(const QVariant &result);
    /*! Sends the response error object to the peer object, if it still exists.
      Use this method when you wants send an error response.
      @warning use this method when the object is in null state won't do
      anything
      @sa isNull
      */
    void error(const Error &error);

private:
    QPointer<Peer> peer;

    QString m_method;

    QVariant m_params;

    bool m_hasId;
    QVariant m_id;
};

} // namespace Phobos

#endif // PHOBOS_RESPONSEHANDLER_H
