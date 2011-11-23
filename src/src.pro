QT += network
TEMPLATE = lib
INCLUDEPATH += . ../3rdparty/

# qt-json library
HEADERS += ../3rdparty/qt-json/json.h
SOURCES +=../3rdparty/qt-json/json.cpp

HEADERS += error-inl.h httphelper.h peer.h responsehandler.h tcphelper.h
SOURCES += httphelper.cpp peer.cpp responsehandler.cpp tcphelper.cpp
