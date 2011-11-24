#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "phobos/tcphelper.h"

namespace Ui {
    class MainWindow;
}

namespace JsonRPC {
    class TcpHelper;
}

class QTcpSocket;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QTcpSocket *socket, QWidget *parent = 0);
    ~MainWindow();

signals:
    void disconnected();
    void closed();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void onReadyResponse(const QVariant &result, const QVariant &id);
    void onRequestError(int code, const QString &message,
                        const QVariant &data, const QVariant &id);
    void onReadyRequest(QSharedPointer<JsonRPC::ResponseHandler> responseHandler);

    void on_newButton_clicked();

    void on_deleteButton_clicked();

    void on_callButton_clicked();

    void on_requestsListWidget_itemActivated(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    JsonRPC::TcpHelper *peer;
    QHash< QVariant, QSharedPointer<JsonRPC::ResponseHandler> > handlers;
};

#endif // MAINWINDOW_H
