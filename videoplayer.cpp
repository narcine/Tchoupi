/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "videoplayer.h"

#include <QtWidgets>
#include <QVideoWidget>
#include <QDebug>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    _mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    QVideoWidget *videoWidget = new QVideoWidget;

    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, &QAbstractButton::clicked, this, &VideoPlayer::openFile);

    _playButton = new QPushButton;
    _playButton->setEnabled(false);
    _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(_playButton, &QAbstractButton::clicked,
            this, &VideoPlayer::play);

    _positionSlider = new QSlider(Qt::Horizontal);
    _positionSlider->setRange(0, 0);

    connect(_positionSlider, &QAbstractSlider::sliderMoved,
            this, &VideoPlayer::setPosition);

    _errorLabel = new QLabel;
    _errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(_playButton);
    controlLayout->addWidget(_positionSlider);

    auto qLabelSR = new QLabel;
    auto qLabelSL = new QLabel;
    _labelLayout = new QHBoxLayout;
    _labelLayout->setSpacing(2);
    _labelLayout->setMargin(0);

    QBoxLayout *arraylabelLayout = new QHBoxLayout;
    arraylabelLayout->addWidget(qLabelSR);
    arraylabelLayout->setStretch(0,50);
    arraylabelLayout->addLayout(_labelLayout);
    arraylabelLayout->addWidget(qLabelSL);
    arraylabelLayout->setStretch(2,50);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addLayout(arraylabelLayout);
    layout->addLayout(controlLayout);
    layout->addWidget(_errorLabel);

    setLayout(layout);

    _mediaPlayer->setVideoOutput(videoWidget);
    _mediaPlayer->setNotifyInterval(100);
    connect(_mediaPlayer, &QMediaPlayer::stateChanged,
            this, &VideoPlayer::mediaStateChanged);
    connect(_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);
    connect(_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::durationChanged);
    connect(_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &VideoPlayer::handleError);
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Movie"));
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
                            .value(0, QDir::homePath()));

    fileDialog.resize(this->width() * 3 / 5, this->height()* 3/ 4);
    if (fileDialog.exec() == QDialog::Accepted)
        setUrl(fileDialog.selectedUrls().constFirst());
}

void VideoPlayer::setUrl(const QUrl &url)
{
    static QString translationPath = QProcessEnvironment::systemEnvironment()
            .value("TRANSLATION_PATH", ".");
    _errorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    int pos = url.toLocalFile().lastIndexOf(".");
    QString moviePath = url.toLocalFile().left(pos);
    pos = url.toLocalFile().lastIndexOf("-");
    int tpos = url.toLocalFile().lastIndexOf(".");
    QString lang = url.toLocalFile().mid(pos+1,tpos-pos-1);

    _readSrt = new ReadSrt(QString(moviePath + ".srt"));
    _readDic = new ReadDic(QString(moviePath + ".dic"));
    _readWords = new ReadDic(QString(translationPath + "/aw_" + lang + ".dic"));
    _mediaPlayer->setMedia(url);
    _playButton->setEnabled(true);
}

void VideoPlayer::play()
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

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state)
    {
        case QMediaPlayer::PlayingState:
            _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
        default:
            _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

ReadSrt::subId VideoPlayer::findSubId(const qint64& position)
{
    QMap<qint64, qint64> subDuration(_readSrt->getSubDuration());
    QMapIterator<ReadSrt::subId, qint64> subPositionI(_readSrt->getSubPosition());
    ReadSrt::subId id = -1;
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

SubLabel* VideoPlayer::createSubLabel(const QString& text, const QString& tooltip)
{
    SubLabel* sub = new SubLabel(text);
    sub->setToolTip(tooltip);
    connect(sub, SIGNAL(enter()), this, SLOT(enterSub()));
    return sub;
}

QLabel* VideoPlayer::findTranslation(const ReadSrt::subId& id, const QString& words)
{
    QLabel *label { nullptr };
    static QString DEFAULT { "default" };
    if(_readDic->getDictionary().contains(words))
    {
        QMap<QString,QString> allTranslation = _readDic->getDictionary()[words];

        if(allTranslation.contains(QString::number(id)))
        {
            label = createSubLabel(words, allTranslation[QString::number(id)]);
        }
        else if (allTranslation.contains(DEFAULT))
        {
            label = createSubLabel(words, allTranslation[DEFAULT]);
        }
        else
        {
            label = new QLabel(words);
        }
    }
    else if(_readWords->getDictionary().contains(words))
    {
        QMap<QString,QString> allTranslation = _readWords->getDictionary()[words];
        if(allTranslation.contains(DEFAULT))
        {
            label = createSubLabel(words, allTranslation[DEFAULT]);
        }
    }
    return label;
}

void VideoPlayer::clearLayout(QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        child->widget()->hide();
        QTimer::singleShot(1000, this, [child] (){
            delete child->widget();
        });
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    static ReadSrt::subId currentId =  -1;
    static ReadSrt::subId oldId =  -1;

    _positionSlider->setValue(static_cast<int>(position));

    currentId = findSubId(position);
    if(currentId == -1)
    {
        clearLayout(_labelLayout);
    }
    else if(oldId != currentId && currentId != -1)
    {
        oldId = currentId;
        QStringList subSplit = _readSrt->getSub()[currentId].split(" ");
        int currentPosition = 0;
        int count = subSplit.size() + 1;
        QVector<QLabel*> currentSub;

        // example : This is a subtitle
        // lfs-iterX = in the X iteration looks for the translation
        // lfs-iter1 : This is a subtitle / lfs-iter2 : This a / lfs-iter3 : This
        // lfs-iter4 : is a subtitle / lfs-iter5 : is a / lfs-iter6 : is
        // lfs-iter7 : a subtitle / lfs-iter8 : a
        // lfs-iter9 : subtitle
        do
        {
            count --;
            QString sentence = "";
            for(int i = currentPosition; i < count; i++)
                sentence = sentence + subSplit[i] + " ";

            sentence = sentence.trimmed().toLower();

            QLabel * translation = findTranslation(currentId, sentence);

            if(count - currentPosition == 1 || translation)
            {
                currentSub.append(translation ? translation : new QLabel(sentence));
                currentPosition = count;
                count = subSplit.size() + 1;
            }

        }while(currentPosition < subSplit.size());

        clearLayout(_labelLayout);

        for(int i = 0; i < currentSub.size(); i++)
            _labelLayout->addWidget(currentSub[i]);
    }
}

void VideoPlayer::enterSub()
{
    _mediaPlayer->pause();
}

bool VideoPlayer::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
       play();
    }
    return QWidget::event(event);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    _positionSlider->setRange(0, static_cast<int>(duration));
}

void VideoPlayer::setPosition(int position)
{
    _mediaPlayer->setPosition(position);
}

void VideoPlayer::handleError()
{
    _playButton->setEnabled(false);
    const QString errorString = _mediaPlayer->errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(_mediaPlayer->error()));
    else
        message += errorString;
    _errorLabel->setText(message);
}
