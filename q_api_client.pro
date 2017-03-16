QT += core network
QT -= gui

CONFIG += c++11

TARGET = q_api_client
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    QNetRequest.cpp \
    QNetRequestSender.cpp \
    QAsrApi.cpp

HEADERS += \
    QNetRequest.h \
    QNetRequestSender.h \
    QAsrApi.h \
    QAsrApiDefine.h

DISTFILES += \
    README.md
