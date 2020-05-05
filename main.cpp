#include "videoplayer.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Tchoupi");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
    QCoreApplication::setApplicationVersion("1.0");

    VideoPlayer player;

    const QScreen *screen = QGuiApplication::screens()[0];
    const QRect availableGeometry = screen->availableGeometry();
    player.resize(availableGeometry.width() * 3 / 5, availableGeometry.height()* 3/ 4);
    player.show();

    return app.exec();
}
