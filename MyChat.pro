#-------------------------------------------------
#
# Project created by QtCreator 2016-08-13T14:18:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyChat
TEMPLATE = app


SOURCES += main.cpp\
        mychat.cpp

HEADERS  += mychat.h

FORMS    += mychat.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    app.rc
RC_FILE += \
    app.rc
