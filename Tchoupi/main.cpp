#include "videoplayer.h"
#include "videoplayergui.h"

#include <QApplication>
#include <QScreen>
#include <QVideoWidget>
#include <QWebEngineSettings>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QImageReader>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Tchoupi");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
    QCoreApplication::setApplicationVersion("2.0");

    VideoPlayerGui gui;
    VideoPlayer player;
    QWidget::connect(&player, SIGNAL(setErrorLabel(const QString&)), &gui, SLOT(setErrorLabel(const QString&)));
    QWidget::connect(&player, SIGNAL(setMedia(const QUrl&)), &gui, SLOT(setMedia(const QUrl&)));
    QWidget::connect(&player, SIGNAL(clearLabelLayout()), &gui, SLOT(clearLabelLayout()));
    QWidget::connect(&player, SIGNAL(addNewLabel(QLabel*)), &gui, SLOT(addNewLabel(QLabel*)));
    QWidget::connect(&player, SIGNAL(pauseVideoPlayer()), &gui, SLOT(pauseVideoPlayer()));
    QWidget::connect(&player, SIGNAL(setIcon(const QString&, const QString&)), &gui, SLOT(setIcon(const QString&, const QString&)));

    QWidget::connect(&gui, SIGNAL(closed()), &player, SLOT(closed()));


    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);


    gui.setOpenWebSiteCallback(std::bind(&VideoPlayer::openWebSite, &player));
    gui.setOpenFileCallback(std::bind(&VideoPlayer::openFile, &player));
    gui.setPositionCallback(std::bind(&VideoPlayer::positionChanged, &player,
        std::placeholders::_1));

    gui.show();

    return app.exec();
}
