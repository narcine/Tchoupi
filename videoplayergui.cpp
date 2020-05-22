#include <QStyle>
#include <QScreen>
#include <QTimer>

#include "videoplayergui.h"
#include "ui_videoplayergui.h"

VideoPlayerGui::VideoPlayerGui(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::VideoPlayerGui)
{
    _ui->setupUi(this);
    _mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    _mediaPlayer->setVideoOutput(_ui->videoWidget);
    _mediaPlayer->setNotifyInterval(100);
    _ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    const QScreen *screen = QGuiApplication::screens()[0];
    const QRect availableGeometry = screen->availableGeometry();
    setLayout(_ui->mainLayout);
    this->resize(availableGeometry.width() * 3 / 5, availableGeometry.height()* 3/ 4);

    connect(_ui->playButton, &QAbstractButton::clicked, this,
            &VideoPlayerGui::play);
    connect(_ui->slider, &QAbstractSlider::sliderMoved, this,
            &VideoPlayerGui::setPosition);
    connect(_mediaPlayer, &QMediaPlayer::stateChanged, this,
            &VideoPlayerGui::mediaStateChanged);
    connect(_mediaPlayer, &QMediaPlayer::durationChanged, this,
            &VideoPlayerGui::durationChanged);
    connect(_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &VideoPlayerGui::handleError);
    connect(_ui->openButton, &QAbstractButton::clicked, this,
            &VideoPlayerGui::openFile);
    connect(_mediaPlayer, &QMediaPlayer::positionChanged, this,
            &VideoPlayerGui::positionChanged);
}

VideoPlayerGui::~VideoPlayerGui()
{
    delete _ui;
}

bool VideoPlayerGui::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
       play();
    }
    return QWidget::event(event);
}

void VideoPlayerGui::play()
{
    switch (_mediaPlayer->state())
    {
        case QMediaPlayer::PlayingState:
            _mediaPlayer->pause();
            break;
        default:
            _mediaPlayer->play();
            break;
    }
}

void VideoPlayerGui::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state)
    {
        case QMediaPlayer::PlayingState:
            _ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
        default:
            _ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayerGui::durationChanged(qint64 duration)
{
    _ui->slider->setRange(0, static_cast<int>(duration));
}

void VideoPlayerGui::setPosition(int position)
{
    _mediaPlayer->setPosition(position);
}

void VideoPlayerGui::handleError()
{
    _ui->playButton->setEnabled(false);
    const QString errorString = _mediaPlayer->errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(_mediaPlayer->error()));
    else
        message += errorString;
    _ui->errorLabel->setText(message);
}

void VideoPlayerGui::setOpenFileCallback(std::function<void()> f)
{
    _openFileCallback = f;
}

void VideoPlayerGui::setPositionCallback(std::function<void(qint64)> f)
{
    _positionCallback = f;
}

void VideoPlayerGui::openFile()
{
     _openFileCallback();
}

void VideoPlayerGui::positionChanged(qint16 position)
{
    _ui->slider->setValue(static_cast<int>(position));
    _positionCallback(position);
}

void VideoPlayerGui::eraseErrorLabel()
{
    _ui->errorLabel->setText(QString());
}

void VideoPlayerGui::setMedia(const QUrl& url)
{
    _mediaPlayer->setMedia(url);
    _ui->playButton->setEnabled(true);
}

void VideoPlayerGui::clearLabelLayout()
{
    QLayoutItem *child;
    while ((child = _ui->labelLayout->takeAt(0)) != nullptr)
    {
        child->widget()->hide();
        QTimer::singleShot(1000, this, [child] (){
            delete child->widget();
        });
    }
}

void VideoPlayerGui::addNewLabel(QLabel* label)
{
    _ui->labelLayout->addWidget(label);
}

void VideoPlayerGui::pauseVideoPlayer()
{
    _mediaPlayer->pause();
}
