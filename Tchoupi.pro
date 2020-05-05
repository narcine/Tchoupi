QT += core gui multimedia widgets multimediawidgets
TARGET = Tchoupi
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
    main.cpp \
    videoplayer.cpp \
    readdic.cpp \
    readsrt.cpp \
    sublabel.cpp

HEADERS += \
    videoplayer.h \
    readdic.h \
    readsrt.h \
    sublabel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
