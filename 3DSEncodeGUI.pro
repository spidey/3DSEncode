#-------------------------------------------------
#
# Project created by QtCreator 2011-12-09T21:29:26
#
#-------------------------------------------------

QT       += core gui

TARGET = 3DSEncodeGUI
TEMPLATE = app

TRANSLATIONS += 3DSEncodeGUI_en.ts\
                3DSEncodeGUI_es.ts\
                3DSEncodeGUI_pt.ts

SOURCES += main.cpp\
            q3dsencodegui.cpp

HEADERS  += q3dsencodegui.h

FORMS    += q3dsencodegui.ui

OTHER_FILES +=

DESTDIR = ~/3DSEncodeGUI_Release/

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    TARGET = 3DSEncodeGUI_debug
}
