#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace JsonRPC {
    class Peer;
    class ResponseHandler;
}

class QTcpServer;
class QTcpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onReadyRequest(const QSharedPointer<JsonRPC::ResponseHandler> &handler);
    void onReadyResponseMessage(const QByteArray &message);

    void onReadyRequestMessage(const QByteArray &message);
    void onReadyResponse(const QVariant &result,const QVariant &id);

    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

    void onButtonClick();
    void onClientSocketConnected();
    void onClientSocketReadyRead();
    void onClientDisconnected();

private:
    Ui::MainWindow *ui;

    JsonRPC::Peer *peer;

    QTcpServer *serverSocket;
    QTcpSocket *socket;
    QByteArray buffer;
    quint8 messageSize;

    QTcpSocket *clientSocket;
    QByteArray clientBuffer;
    quint8 clientMessageSize;
};

#endif // MAINWINDOW_H
