TARGET = phobosnode
TEMPLATE = app

include(../../qt-json-rpc.pri)

SOURCES += main.cpp  mainwindow.cpp tcpserverwindow.cpp
HEADERS += mainwindow.h tcpserverwindow.h
FORMS += mainwindow.ui tcpserverwindow.ui
