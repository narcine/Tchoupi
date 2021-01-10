#include <QStyle>
#include <QScreen>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>

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
    _ui->slider->setMinimum(0);
    _ui->slider->setRange(0,0);

    const QScreen *screen = QGuiApplication::screens()[0];
    const QRect availableGeometry = screen->availableGeometry();
    setLayout(_ui->mainLayout);
    this->resize(availableGeometry.width() * 3 / 5, availableGeometry.height()* 3/ 4);

    connect(_ui->slider, &Slider::positionChanged, this, &VideoPlayerGui::setPosition);
    connect(_ui->playButton, &QAbstractButton::clicked, this,
            &VideoPlayerGui::play);
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
    connect(_ui->fromOpensubtitlesAct, &QAction::triggered, this,
            &VideoPlayerGui::openWebSite);
}

void VideoPlayerGui::setPosition(qint64 position)
{
    _mediaPlayer->setPosition(position);
}

void VideoPlayerGui::closeEvent(QCloseEvent* /* event */)
{
    emit closed();
}

VideoPlayerGui::~VideoPlayerGui()
{
    delete _ui;
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

void VideoPlayerGui::setIcon(const QString& icon1, const QString& icon2)
{
    QImage image1(":"+icon1);
    QImage image2(":"+icon2);

    QPainter pngPainter;
    QImage image(QSize(380, 380), QImage::Format_ARGB32);
    QGraphicsPixmapItem item1( QPixmap::fromImage(image1));
    QGraphicsPixmapItem item2( QPixmap::fromImage(image2));
    QGraphicsScene* scene = new QGraphicsScene;
    scene->addItem(&item1);
    scene->addItem(&item2);
    item1.setPos(0,0);
    item2.setPos(10,10);

    pngPainter.begin(&image);
    scene->render(&pngPainter);
    pngPainter.end();

    setWindowIcon(QIcon(QPixmap::fromImage(image)));
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

void VideoPlayerGui::setOpenWebSiteCallback(std::function<void()> f)
{
    _openWebSiteCallback = f;
}

void VideoPlayerGui::positionChanged(qint64 position)
{
    _ui->slider->setValue(static_cast<int>(position));
    _positionCallback(position);
}

void VideoPlayerGui::setErrorLabel(const QString& text)
{
    _ui->errorLabel->setText(text);
}

void VideoPlayerGui::setMedia(const QUrl& url)
{
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());

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

void VideoPlayerGui::openWebSite()
{
   _openWebSiteCallback();
}
