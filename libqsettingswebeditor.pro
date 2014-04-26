CONFIG  += debug

QT -= gui
QT += network xml

TEMPLATE = lib
TARGET   = qsettingswebeditor

DEFINES += LIBQSETTINGSWEBEDITOR_LIBRARY

SOURCES += qsettingswebeditor.cpp
HEADERS += qsettingswebeditor.h
   LIBS += -lqhttpserver

unix {
    target.path = /usr/lib
    INSTALLS += target

    header_files.files = $$HEADERS
    header_files.path = /usr/include
    INSTALLS += header_files
}
