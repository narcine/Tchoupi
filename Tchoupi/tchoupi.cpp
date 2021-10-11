#include <QStyle>
#include <QScreen>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QGraphicsPixmapItem>

#include "tchoupi.h"
#include "ui_tchoupi.h"

Tchoupi::Tchoupi(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::Tchoupi)
{
    _ui->setupUi(this);
    _mediaPlayer = std::make_unique<QMediaPlayer>(this, QMediaPlayer::VideoSurface);
    _mediaPlayer->setVideoOutput(_ui->videoWidget);
    _mediaPlayer->setNotifyInterval(100);
    _ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    _ui->slider->setRange(0,0);

    // Initialize the screen's size
    const QScreen *screen = QGuiApplication::screens()[0];
    q_check_ptr(screen);
    const QRect availableGeometry = screen->availableGeometry();
    this->resize(availableGeometry.width() * 3 / 5, availableGeometry.height()* 3/ 4);

    // Retrieve the country codes
    QFile countryCodeFile(":countryCode.json");
    countryCodeFile.open(QFile::ReadOnly | QFile::Text);
    _countryCodes = QJsonDocument::fromJson(countryCodeFile.readAll()).object();
    countryCodeFile.close();

    // Connect the graphical components
    connect(_ui->slider, &Slider::positionChanged, this, &Tchoupi::setPosition);
    connect(_ui->playButton, &QAbstractButton::clicked, this,
            &Tchoupi::handlePlayButton);
    connect(_mediaPlayer.get(), &QMediaPlayer::stateChanged, this,
            &Tchoupi::mediaStateChanged);
    connect(_mediaPlayer.get(), &QMediaPlayer::durationChanged, this,
            &Tchoupi::durationChanged);
    connect(_mediaPlayer.get(), QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &Tchoupi::handleError);
    connect(_ui->openButton, &QAbstractButton::clicked, this,
            &Tchoupi::openFile);
    connect(_mediaPlayer.get(), &QMediaPlayer::positionChanged, this,
            &Tchoupi::positionChanged);

    // Set the layout
    setLayout(_ui->mainLayout);

}

void Tchoupi::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Movie"));
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
                            .value(0, QDir::homePath()));

    fileDialog.resize(this->width() * 3 / 5, this->height()* 3/ 4);
    if (fileDialog.exec() == QDialog::Accepted)
    {
        setErrorLabel(Errors::ERR_NONE);
        setUrl(fileDialog.selectedUrls().constFirst());
    }
}

void Tchoupi::setPosition(qint64 position)
{
    _mediaPlayer->setPosition(position);
}

void Tchoupi::keyPressEvent(QKeyEvent* /* event */)
{
    handlePlayButton();
}

Tchoupi::~Tchoupi()
{
    delete _ui;
}

void Tchoupi::handlePlayButton()
{
    switch(_mediaPlayer->state())
    {
        case QMediaPlayer::PlayingState:
            _mediaPlayer->pause();
            break;
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            _mediaPlayer->play();
            break;
        default:
            break;
    }
}

void Tchoupi::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state)
    {
        case QMediaPlayer::PlayingState:
            _ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            _ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        default:
            break;
    }
}

void Tchoupi::setUrl(const QUrl &url)
{
    QFileInfo localFile = url.toLocalFile();
    QString moviePath = localFile.path();
    QString movieName = localFile.fileName().replace("." + localFile.suffix(), "");
    int pos = movieName.lastIndexOf("-");
    int tpos = movieName.lastIndexOf(".");
    QString check = movieName.mid(pos+1,tpos-pos-1);
    QString country1, country2;
    if(checkLangs(check, country1, country2))
    {
        static QString translationPath = QProcessEnvironment::systemEnvironment()
                .value("TRANSLATION_PATH", ".");
        static QString videoTranslationPath = QProcessEnvironment::systemEnvironment()
                .value("VIDEO_TRANSLATION_PATH", moviePath);
        QString lang1 = _countryCodes[country1].toObject()["code"].toString();
        QString lang2 = _countryCodes[country2].toObject()["code"].toString();
        _readSrt = std::make_unique<ReadSrt>(QString(moviePath + "/" + movieName + ".srt"));
        _readDic = std::make_unique<ReadDic>(QString(videoTranslationPath + "/" + movieName + ".dic"));
        _readWords = std::make_unique<ReadDic>(QString(translationPath + "/aw_" + lang1 + lang2 + ".dic"));
        setMedia(url);
        setIcon(_countryCodes[country1].toObject()["image"].toString(),
            _countryCodes[country2].toObject()["image"].toString());
    }
}

bool Tchoupi::checkLangs(const QString& sentence, QString& country1, QString& country2)
{
    country1.clear();
    country2.clear();
    QStringList listCountry = _countryCodes.keys();
    for(QString country : listCountry)
    {
        QJsonObject object =  _countryCodes[country].toObject();
        int index = sentence.indexOf(object["code"].toString());
        if(index == 0)
        {
            country1 = country;
        }
        else if(index > 0)
        {
            country2 = country;
        }
        if(!country1.isEmpty() && !country2.isEmpty())
        {
            break;
        }
    }
    if(country1.isEmpty() || country2.isEmpty()) setErrorLabel(Errors::ERR_UNKNOWN_LANG);
    if(country1 == country2) setErrorLabel(Errors::ERR_LANG_DB);
    return  !country1.isEmpty() && !country2.isEmpty() && country1 != country2;
}

ReadSrt::subId Tchoupi::findSubId(ReadSrt* const readSrt, const qint64& position)
{
    q_check_ptr(readSrt);

    ReadSrt::subId id = -1;
    QMap<qint64, qint64> subDuration(readSrt->getSubDuration());
    QMapIterator<ReadSrt::subId, qint64> subPositionI(readSrt->getSubPosition());

    while(subPositionI.hasNext())
    {
        subPositionI.next();
        if(subPositionI.value() <= position && subDuration[subPositionI.value()] >= position)
        {
                id =  subPositionI.key();
                break;
        }
    }
    return id;
}

SubLabel* Tchoupi::createSubLabel(const QString& text, const QString& tooltip)
{
    static bool notPaused = false;
    SubLabel* sub = new SubLabel(text);
    sub->setToolTip(tooltip);
    connect(sub, &SubLabel::enter, this, [&](){ if(_mediaPlayer->state() == QMediaPlayer::PausedState) notPaused = true; else _mediaPlayer->pause(); });
    connect(sub, &SubLabel::leave, this, [&](){ if(!notPaused)_mediaPlayer->play(); notPaused = false; });
    return sub;
}

QLabel* Tchoupi::findTranslation(const QString& sentence, const ReadSrt::subId& id)
{
    QLabel *label { nullptr };
    static QString DEFAULT { "default" };
    if(_readDic->getDictionary().contains(sentence))
    {
        QMap<QString,QString> allTranslation = _readDic->getDictionary()[sentence];

        if(allTranslation.contains(QString::number(id)))
        {
            label = createSubLabel(sentence, allTranslation[QString::number(id)]);
        }
        else if (allTranslation.contains(DEFAULT))
        {
            label = createSubLabel(sentence, allTranslation[DEFAULT]);
        }
        else
        {
            label = new QLabel(sentence);
        }
    }
    else if(_readWords->getDictionary().contains(sentence))
    {
        QMap<QString,QString> allTranslation = _readWords->getDictionary()[sentence];
        if(allTranslation.contains(DEFAULT))
        {
            label = createSubLabel(sentence, allTranslation[DEFAULT]);
        }
    }
    return label;
}

void Tchoupi::addNewLabel(QLabel* label)
{
    _ui->labelLayout->addWidget(label);
}

void Tchoupi::updateSubtitle(qint64 position)
{
    static ReadSrt::subId oldId =  -1;

    ReadSrt::subId currentId = _readSrt? findSubId(_readSrt.get(), position) :  -1;
    if(currentId == -1) { clearLabelLayout(); return; }

    if(oldId != currentId && currentId != -1)
    {
        oldId = currentId;
        QStringList subSplit = _readSrt->getSub()[currentId].split(" ");
        int currentPosition = 0;
        int count = subSplit.size() + 1;
        QVector<QLabel*> currentSub;

        // example : This is a subtitle
        // lfs-iterX = in the X iteration looks for the translation
        // lfs-iter1 : This is a subtitle / lfs-iter2 : This is a / lfs-iter3 : This is
        // lfs-iter4 : This / lfs-iter5 : is a subtitle / lfs-iter6 : is a / lfs-iter7 : is
        // lfs-iter8 : a subtitle / lfs-iter9 : a
        // lfs-iter10 : subtitle
        do
        {
            count --;
            QString sentence = "";
            for(int i = currentPosition; i < count; i++)
                sentence = sentence + subSplit[i] + " ";

            sentence = sentence.trimmed().toLower();

            QLabel* translation = findTranslation(sentence, currentId);

            if(count - currentPosition == 1 || translation)
            {
                currentSub.append(translation ? translation : new QLabel(sentence));
                currentPosition = count;
                count = subSplit.size() + 1;
            }

        }while(currentPosition < subSplit.size());

        clearLabelLayout();

        for(int i = 0; i < currentSub.size(); i++)
            addNewLabel(currentSub[i]);
    }
}

void Tchoupi::durationChanged(qint64 duration)
{
   _ui->slider->setRange(0, static_cast<int>(duration));
   qint64 hour = duration/(3600*1000);
   qint64 min = (duration%(3600*1000))/(60*1000);
   qint64 sec =(duration%(3600*1000))%(60*1000)/1000;
   _totalDuration = QString("%1:%2:%3").arg(hour, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0'))
        .arg(sec, 2, 10, QChar('0'));
}

void Tchoupi::setIcon(const QString& country1, const QString& country2)
{
    QImage image1(":"+country1);
    QImage image2(":"+country2);

    QPainter pngPainter;
    QImage image(QSize(380, 380), QImage::Format_ARGB32);
    QGraphicsPixmapItem item1( QPixmap::fromImage(image1));
    QGraphicsPixmapItem item2( QPixmap::fromImage(image2));
    static QGraphicsScene scene;
    scene.addItem(&item1);
    scene.addItem(&item2);
    item1.setPos(0,0);
    item2.setPos(10,10);

    pngPainter.begin(&image);
    scene.render(&pngPainter);
    pngPainter.end();

    setWindowIcon(QIcon(QPixmap::fromImage(image)));
}

void Tchoupi::handleError()
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

void Tchoupi::positionChanged(qint64 position)
{
    _ui->slider->setValue(static_cast<int>(position));
    qint64 hour = position/(3600*1000);
    qint64 min = (position%(3600*1000))/(60*1000);
    qint64 sec =(position%(3600*1000))%(60*1000)/1000;
    QString currDuration = QString("%1:%2:%3").arg(hour, 2, 10, QChar('0'))
        .arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
    _ui->duration->setText(currDuration + "/" + _totalDuration);
    updateSubtitle(position);
}

void Tchoupi::setErrorLabel(Errors err)
{
    _ui->errorLabel->setText(_errors[err]);
}

void Tchoupi::setMedia(const QUrl& url)
{
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());

    _mediaPlayer->setMedia(url);
    _mediaPlayer->pause();
    _ui->playButton->setEnabled(true);
}

void Tchoupi::clearLabelLayout()
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
