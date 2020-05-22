#include "videoplayer.h"

#include <QApplication>
#include <QScreen>
#include "videoplayergui.h"
#include <QVideoWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Tchoupi");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
    QCoreApplication::setApplicationVersion("1.0");

    VideoPlayerGui gui;
    VideoPlayer player;
    QWidget::connect(&player, SIGNAL(eraseErrorLabel()), &gui, SLOT(eraseErrorLabel()));
    QWidget::connect(&player, SIGNAL(setMedia(const QUrl&)), &gui, SLOT(setMedia(const QUrl&)));
    QWidget::connect(&player, SIGNAL(clearLabelLayout()), &gui, SLOT(clearLabelLayout()));
    QWidget::connect(&player, SIGNAL(addNewLabel(QLabel*)), &gui, SLOT(addNewLabel(QLabel*)));
    QWidget::connect(&player, SIGNAL(pauseVideoPlayer()), &gui, SLOT(pauseVideoPlayer()));

    gui.setOpenFileCallback(std::bind(&VideoPlayer::openFile, &player));
    gui.setPositionCallback(std::bind(&VideoPlayer::positionChanged, &player,
        std::placeholders::_1));

    gui.show();

    return app.exec();
}
