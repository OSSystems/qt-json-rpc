#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "phobos/peer.h"
#include "phobos/responsehandler.h"
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include <QUuid>
#include <QIntValidator>

using namespace JsonRPC;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    peer(new Peer(this)),
    serverSocket(new QTcpServer(this)),
    socket(NULL),
    clientSocket(new QTcpSocket(this)),
    clientMessageSize(0)
{
    ui->setupUi(this);

    connect(peer, SIGNAL(readyRequest(QSharedPointer<JsonRPC::ResponseHandler>)),
            this, SLOT(onReadyRequest(QSharedPointer<JsonRPC::ResponseHandler>)));
    connect(peer, SIGNAL(readyResponseMessage(QByteArray)),
            this, SLOT(onReadyResponseMessage(QByteArray)));

    connect(peer, SIGNAL(readyRequestMessage(QByteArray)),
            this, SLOT(onReadyRequestMessage(QByteArray)));
    connect(peer, SIGNAL(readyResponse(QVariant,QVariant)),
            this, SLOT(onReadyResponse(QVariant,QVariant)));

    connect(serverSocket, SIGNAL(newConnection()),
            this, SLOT(onNewConnection()));

    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT(onButtonClick()));
    connect(clientSocket, SIGNAL(connected()),
            this, SLOT(onClientSocketConnected()));
    connect(clientSocket, SIGNAL(readyRead()),
            this, SLOT(onClientSocketReadyRead()));
    connect(clientSocket, SIGNAL(disconnected()),
            this, SLOT(onClientDisconnected()));

    serverSocket->listen(QHostAddress::Any);
    ui->lineEdit->setText(QString::number(serverSocket->serverPort()));

    ui->lineEdit_2->setValidator(new QIntValidator(ui->lineEdit_2));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReadyRequest(const QSharedPointer<JsonRPC::ResponseHandler> &handler)
{
    qDebug("ready request");
    if (handler->method() == "echo") {
        handler->response("okay");

        QString text;
        QDebug dbg(&text);
        dbg << handler->params();
        QMessageBox::information(this, "Echo", text, QMessageBox::Ok);
    } else {
        handler->error(Error(ErrorCode::METHOD_NOT_FOUND));
    }
}

void MainWindow::onReadyResponseMessage(const QByteArray &message)
{
    qDebug("ready response message");
    QDataStream stream(socket);
    quint8 messageSize = message.size();
    stream << messageSize;
    socket->write(message);
}

void MainWindow::onReadyRequestMessage(const QByteArray &message)
{
    qDebug("ready request message");
    QDataStream stream(clientSocket);
    quint8 messageSize = message.size();
    stream << messageSize;
    clientSocket->write(message);
}

void MainWindow::onReadyResponse(const QVariant &result, const QVariant &id)
{
    qDebug("----result:");
    qDebug() << id;
    qDebug() << result;
    qDebug("--");
    clientSocket->disconnectFromHost();
}

void MainWindow::onNewConnection()
{
    qDebug("new connection");
    if (socket) {
        delete serverSocket->nextPendingConnection();
        return;
    }

    socket = serverSocket->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(onDisconnected()));
    buffer.clear();
    messageSize = 0;
}

void MainWindow::onReadyRead()
{
    qDebug("ready read");
    if (!messageSize) {
        QDataStream stream(socket);
        stream >> messageSize;
    }

    buffer.append(socket->readAll());
    if (messageSize
            && buffer.size() >= messageSize) {
        peer->handleMessage(buffer.left(messageSize));
        buffer.remove(0, messageSize);

        if (buffer.size()) {
            QDataStream stream(socket);
            stream >> messageSize;
            buffer.append(socket->readAll());
        } else {
            messageSize = 0;
        }
    }
}

void MainWindow::onDisconnected()
{
    qDebug("disconnected");
    socket->deleteLater();
    socket = NULL;
}

void MainWindow::onButtonClick()
{
    qDebug("button click");
    clientSocket->connectToHost(QHostAddress::LocalHost, ui->lineEdit_2->text().toInt());
}

void MainWindow::onClientSocketConnected()
{
    qDebug("client socket connected");
    QVariantList params;
    params.push_back("Hello");
    params.push_back("World");
    peer->call("echo", params, QUuid::createUuid().toString());
    ui->pushButton->setEnabled(false);
}

void MainWindow::onClientSocketReadyRead()
{
    qDebug("client socket ready read");
    if (!clientMessageSize) {
        QDataStream stream(clientSocket);
        stream >> clientMessageSize;
    }

    clientBuffer.append(clientSocket->readAll());
    if (clientMessageSize
            && clientBuffer.size() >= clientMessageSize) {
        peer->handleMessage(clientBuffer.left(clientMessageSize));
        clientBuffer.remove(0, clientMessageSize);

        if (clientBuffer.size()) {
            QDataStream stream(clientSocket);
            stream >> clientMessageSize;
            clientBuffer.append(clientSocket->readAll());
        } else {
            clientMessageSize = 0;
        }
    }
}

void MainWindow::onClientDisconnected()
{
    qDebug("client disconnected");
    ui->pushButton->setEnabled(true);
}
