#include "tcpserverwindow.h"
#include "ui_tcpserverwindow.h"
#include <QTcpServer>
#include "mainwindow.h"
#include <QInputDialog>
#include <QTcpSocket>

TcpServerWindow::TcpServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpServerWindow),
    server(new QTcpServer(this))
{
    ui->setupUi(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    server->listen();
    ui->lcdNumber->display(server->serverPort());
}

TcpServerWindow::~TcpServerWindow()
{
    delete ui;
}

void TcpServerWindow::onNewConnection()
{
    MainWindow *w = new MainWindow(server->nextPendingConnection(), this);
    connect(w, SIGNAL(disconnected()), w, SLOT(deleteLater()));
    connect(w, SIGNAL(closed()), w, SLOT(deleteLater()));
    w->show();
}

void TcpServerWindow::on_connectButton_clicked()
{
    QTcpSocket *s = new QTcpSocket(this);
    connect(s, SIGNAL(connected()), this, SLOT(onConnected()));

    const quint16 port = QInputDialog::getInt(this, "port", "port", 0, 0, 65535);
    s->connectToHost(QHostAddress::LocalHost, port);
}

void TcpServerWindow::onConnected()
{
    QTcpSocket *s = qobject_cast<QTcpSocket *>(sender());
    if (s) {
        MainWindow *w = new MainWindow(s, this);
        connect(w, SIGNAL(disconnected()), w, SLOT(deleteLater()));
        connect(w, SIGNAL(closed()), w, SLOT(deleteLater()));
        w->show();
    }
}
