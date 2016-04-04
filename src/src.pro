TEMPLATE = app
TARGET = avdp
QT += widgets

HEADERS = gui.hpp \
    cli.hpp \
    screenshot.hpp \
    recorder.hpp \
    imageblock.hpp \
    util.hpp \
    constants.hpp

SOURCES = main.cpp \
    gui.cpp \
    cli.cpp \
    screenshot.cpp \
    recorder.cpp \
    imageblock.cpp

# See https://doc.qt.io/qt-5/qmake-variable-reference.html#config
CONFIG += qt c++11 warn_on debug_and_release
