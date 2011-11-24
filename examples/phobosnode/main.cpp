#include <QtGui/QApplication>
#include "tcpserverwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServerWindow w;
    w.show();

    return a.exec();
}
