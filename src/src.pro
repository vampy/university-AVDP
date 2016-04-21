TEMPLATE = app
TARGET = avdp
QT += widgets

HEADERS = gui.hpp \
    cli.hpp \
    screenshot.hpp \
    recorder.hpp \
    imageblock.hpp \
    constants.hpp \
    compareframes.hpp \
    util.hpp

SOURCES = main.cpp \
    gui.cpp \
    cli.cpp \
    screenshot.cpp \
    recorder.cpp \
    imageblock.cpp \
    compareframes.cpp \
    util.cpp

# See https://doc.qt.io/qt-5/qmake-variable-reference.html#config
CONFIG += qt c++11 warn_on debug_and_release
