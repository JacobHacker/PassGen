QT       += core gui network

TARGET = qPassGen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README \
    foreign-lang-charsets.txt \
    INSTALL.md
