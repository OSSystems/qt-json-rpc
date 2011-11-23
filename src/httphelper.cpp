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

#include "httphelper.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <qjson/parser.h>

using namespace Phobos;

HttpHelper::HttpHelper(QObject *parent) :
    QObject(parent),
    peer(new Peer(this)),
    httpClient(new QNetworkAccessManager(this))
{
    connect(peer, SIGNAL(readyRequestMessage(QByteArray)),
            this, SLOT(onReadyRequestMessage(QByteArray)));
    connect(peer, SIGNAL(readyResponse(QVariant,QVariant)),
            this, SIGNAL(readyResponse(QVariant,QVariant)));
    connect(peer, SIGNAL(requestError(int,QString,QVariant,QVariant)),
            this, SIGNAL(requestError(int,QString,QVariant,QVariant)));

    connect(httpClient, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

QUrl HttpHelper::url() const
{
    return m_url;
}

void HttpHelper::setUrl(const QUrl &url)
{
    this->m_url = url;
}

bool HttpHelper::call(const QString &method, const QVariant &params, const QVariant &id)
{
    return peer->call(method, params, id);
}

void HttpHelper::onReadyRequestMessage(const QByteArray &json)
{
    QNetworkRequest request(m_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QString("application/json-rpc"));
    request.setRawHeader("Accept", "application/json-rpc");

    httpClient->post(request, json);
}

void HttpHelper::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QByteArray content = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        QJson::Parser parser;
        bool ok;
        parser.parse(content, &ok);

        if (!ok) {
            emit error(reply->error());
            return;
        }
    }

    peer->handleMessage(content);
}
