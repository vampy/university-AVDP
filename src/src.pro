TEMPLATE = app
TARGET = avdp
QT += widgets network

HEADERS = gui.hpp \
    cli.hpp \
    screenshot.hpp \
    recorder.hpp \
    imageblock.hpp \
    constants.hpp \
    compareframes.hpp \
    util.hpp \
    streamingserver.hpp \
    videostreamer.hpp

SOURCES = main.cpp \
    gui.cpp \
    cli.cpp \
    screenshot.cpp \
    recorder.cpp \
    imageblock.cpp \
    compareframes.cpp \
    util.cpp \
    videostreamer.cpp \
    streamingserver.cpp

# See https://doc.qt.io/qt-5/qmake-variable-reference.html#config
CONFIG += qt c++11 warn_on debug_and_release

# See https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
QMAKE_CXXFLAGS += -march=native
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_DEBUG += -Og
