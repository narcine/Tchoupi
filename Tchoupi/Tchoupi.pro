QT += core gui multimedia widgets multimediawidgets
TARGET = Tchoupi
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
    main.cpp \
    slider.cpp \
    readdic.cpp \
    readsrt.cpp \
    sublabel.cpp \
    tchoupi.cpp

HEADERS += \
    errors.h \
    slider.h \
    readdic.h \
    readsrt.h \
    sublabel.h \
    tchoupi.h

FORMS += \
    tchoupi.ui

RESOURCES += \
    data/languages.qrc \

# disable all the Qt APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

