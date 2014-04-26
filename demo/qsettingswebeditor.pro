QT      += network
QT      -= gui

CONFIG  += debug console
CONFIG  -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
   LIBS += ../libqsettingswebeditor.so.1.0.0 -lqhttpserver -lqsharedsettings

TARGET   = qsettingswebeditor

isEmpty(PREFIX) {
 PREFIX = /usr/bin
}

inst.files = qsettingswebeditor
inst.path  = $$PREFIX/

INSTALLS += inst
