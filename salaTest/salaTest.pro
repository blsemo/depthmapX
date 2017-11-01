include(../defaults.pri)
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += ../ThirdParty/Catch ../ThirdParty/FakeIt

SOURCES += main.cpp \
    testentityparsing.cpp \
    testpointmap.cpp \
    testlinkutils.cpp \
    testgridproperties.cpp \
    testisovistdef.cpp \
    testshapegraphs.cpp \
    teststructsizes.cpp \
    testsparksieve.cpp \
    testattributetable.cpp \
    testattributetableindex.cpp \
    testlayermanager.cpp \
    testattributetablehelpers.cpp \
    testattributetableview.cpp

win32:Release:LIBS += -L../genlib/release -L../salalib/release
win32:Debug:LIBS += -L../genlib/debug -L../salalib/debug
!win32:LIBS += -L../genlib -L../salalib

LIBS += -lsalalib -lgenlib
