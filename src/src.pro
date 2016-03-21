TEMPLATE = app
TARGET = pdav
QT += widgets

HEADERS = *.hpp
SOURCES = *.cpp

# See https://doc.qt.io/qt-5/qmake-variable-reference.html#config
CONFIG += qt c++11 warn_on debug_and_release
