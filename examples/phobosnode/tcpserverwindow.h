#ifndef TCPSERVERWINDOW_H
#define TCPSERVERWINDOW_H

#include <QMainWindow>

namespace Ui {
    class TcpServerWindow;
}

class QTcpServer;
//class MainWindow;

class TcpServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpServerWindow(QWidget *parent = 0);
    ~TcpServerWindow();

private slots:
    void onNewConnection();
    void onConnected();

    void on_connectButton_clicked();

private:
    Ui::TcpServerWindow *ui;
    QTcpServer *server;
//    QList<MainWindow *> windows;
};

#endif // TCPSERVERWINDOW_H
