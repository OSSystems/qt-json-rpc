QT += network
INCLUDEPATH += $$PWD/ $$PWD/3rdparty/

# qt-json library
HEADERS += $$PWD/3rdparty/qt-json/json.h
SOURCES += $$PWD/3rdparty/qt-json/json.cpp

HEADERS += $$PWD/error.h \
        $$PWD/httphelper.h \
        $$PWD/peer.h \
        $$PWD/responsehandler.h \
        $$PWD/tcphelper.h

SOURCES += $$PWD/error.cpp \
        $$PWD/httphelper.cpp \
        $$PWD/peer.cpp \
        $$PWD/responsehandler.cpp \
        $$PWD/tcphelper.cpp
