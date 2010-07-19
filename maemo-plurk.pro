#-------------------------------------------------
#
# Project created by QtCreator 2010-07-17T15:43:00
#
#-------------------------------------------------


LIBS += -L/usr/lib -lqjson

QT       += core gui network qjson webkit

TARGET = maemo-plurk
TEMPLATE = app


SOURCES += main.cpp \
    logindialog.cpp \
    plurkview.cpp \
    clicklabel.cpp

HEADERS  += \
    logindialog.h \
    plurkview.h \
    clicklabel.h

CONFIG += mobility
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe489bf86
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

FORMS += \
    logindialog.ui \
    plurkview.ui
