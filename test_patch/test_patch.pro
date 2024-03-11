TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
QT += testlib

CONFIG -= qt

SOURCES += \
        ../post_fire_simulation/patch.cpp \
        test_patch.cpp

HEADERS += \
    ../post_fire_simulation/patch.h \
    catch.hpp
