QT       += core gui network widgets
CONFIG += c++11

TARGET = qPassGen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    foreign-lang-charsets.txt \
    INSTALL.md \
    README.md \
    CHARSETS.md
