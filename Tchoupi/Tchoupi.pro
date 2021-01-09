QT += core gui multimedia widgets multimediawidgets webenginewidgets
TARGET = Tchoupi
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
    main.cpp \
    slider.cpp \
    videoplayer.cpp \
    readdic.cpp \
    readsrt.cpp \
    sublabel.cpp \
    videoplayergui.cpp \
    browser.cpp \
    browserwindow.cpp \
    downloadmanagerwidget.cpp \
    downloadwidget.cpp \
    tabwidget.cpp \
    webpage.cpp \
    webpopupwindow.cpp \
    webview.cpp

HEADERS += \
    slider.h \
    videoplayer.h \
    readdic.h \
    readsrt.h \
    sublabel.h \
    videoplayergui.h \
    browser.h \
    browserwindow.h \
    downloadmanagerwidget.h \
    downloadwidget.h \
    tabwidget.h \
    webpage.h \
    webpopupwindow.h \
    webview.h

FORMS += \
    videoplayergui.ui \
    certificateerrordialog.ui \
    passworddialog.ui \
    downloadmanagerwidget.ui \
    downloadwidget.ui

RESOURCES += data/simplebrowser.qrc \
    data/languages.qrc \
    data/utility.qrc

!win32: LIBS += -lz

# disable all the Qt APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
LIBQUAZIP_VERSION_PREFIX = Qt5
!win32 : LIBS += -L../quazip/quazip -l$${LIBQUAZIP_VERSION_PREFIX}Quazip

