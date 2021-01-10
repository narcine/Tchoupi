#ifndef VIDEOPLAYERGUI_H
#define VIDEOPLAYERGUI_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QLabel>
#include <QGraphicsView>

namespace Ui {
class VideoPlayerGui;
}

class VideoPlayerGui : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerGui(QWidget *parent = nullptr);
    ~VideoPlayerGui();

    void setOpenFileCallback(std::function<void()> f);
    void setPositionCallback(std::function<void(qint64)> f);
    void setOpenWebSiteCallback(std::function<void()> f);

private:
    Ui::VideoPlayerGui* _ui;
    QMediaPlayer* _mediaPlayer;
    QString _totalDuration;
    std::function<void()> _openFileCallback;
    std::function<void(qint64)> _positionCallback;
    std::function<void()> _openWebSiteCallback;

    void closeEvent(QCloseEvent *) override;

private slots:
    void play();
    void mediaStateChanged(QMediaPlayer::State state);
    void durationChanged(qint64 duration);
    void handleError();
    void openFile();
    void setPosition(qint64 position);
    void positionChanged(qint64 position);
    void setErrorLabel(const QString&);
    void setMedia(const QUrl& url);
    void clearLabelLayout();
    void addNewLabel(QLabel* label);
    void pauseVideoPlayer();
    void openWebSite();
    void setIcon(const QString& icon1, const QString& icon2);

signals:
    void closed();
};

#endif // VIDEOPLAYERGUI_H
