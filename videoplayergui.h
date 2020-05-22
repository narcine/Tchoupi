#ifndef VIDEOPLAYERGUI_H
#define VIDEOPLAYERGUI_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QLabel>

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

private:
    Ui::VideoPlayerGui* _ui;
    QMediaPlayer* _mediaPlayer;
    std::function<void()> _openFileCallback;
    std::function<void(qint64)> _positionCallback;

    bool event(QEvent *event) override;

private slots:
    void play();
    void mediaStateChanged(QMediaPlayer::State state);
    void durationChanged(qint64 duration);
    void handleError();
    void setPosition(int position);
    void openFile();
    void positionChanged(qint16 position);
    void eraseErrorLabel();
    void setMedia(const QUrl& url);
    void clearLabelLayout();
    void addNewLabel(QLabel* label);
    void pauseVideoPlayer();
};

#endif // VIDEOPLAYERGUI_H
