QMAKE_CXXFLAGS += -std=c++11
QT -= core gui
TARGET = multitools
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

HEADERS += \
    include/*.h \
    include/defines.h

SOURCES += \
    src/*.cpp

INCLUDEPATH += include

DESTDIR = .
OBJECTS_DIR = build
