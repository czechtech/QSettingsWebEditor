CONFIG  += debug
CONFIG  -= app_bundle
QT      += network xml
QT      -= gui

TEMPLATE = app
CONFIG  += console

SOURCES   += main.cpp qsettingswebeditor.cpp
HEADERS   += qsettingswebeditor.h

unix:LIBS += -lqsharedsettings -lqhttpserver

TARGET   = qsettingswebeditor

isEmpty(PREFIX) {
 PREFIX = /usr/bin
}

inst.files = qsettingswebeditor
inst.path  = $$PREFIX/

INSTALLS += inst
