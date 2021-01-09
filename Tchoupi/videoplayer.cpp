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
#include "ui_videoplayergui.h"
#include "tabwidget.h"
#include "downloadwidget.h"
#include "downloadmanagerwidget.h"

#include <QtWidgets>
#include <QVideoWidget>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>


VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent), _browser(nullptr), _window(nullptr)
{
    QFile countryCodeFile(":countryCode.json");
    QFile errorFile(":errors.json");
    countryCodeFile.open(QFile::ReadOnly | QFile::Text);
    errorFile.open(QFile::ReadOnly | QFile::Text);
    _countryCode = QJsonDocument::fromJson(countryCodeFile.readAll()).object();
    _errors = QJsonDocument::fromJson(errorFile.readAll()).object();
    countryCodeFile.close();
    errorFile.close();
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
    emit setErrorLabel(tr(""));
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
        _lang1 = _countryCode[country1].toObject()["code"].toString();
        _lang2 = _countryCode[country2].toObject()["code"].toString();
        _readSrt = QSharedPointer<ReadSrt>(new ReadSrt(QString(moviePath + "/" + movieName + ".srt")));
        _readDic = QSharedPointer<ReadDic>(new ReadDic(QString(videoTranslationPath + "/" + movieName + ".dic")));
        _readWords = QSharedPointer<ReadDic>(new ReadDic(QString(translationPath + "/aw_" + _lang1 + _lang2 + ".dic")));
        _mediaInfo = QFileInfo(url.toLocalFile());
        QFileInfo f(url.toLocalFile());
        emit setMedia(url);
        emit setIcon(_countryCode[country1].toObject()["image"].toString(), _countryCode[country2].toObject()["image"].toString());
    }
}

bool VideoPlayer::checkLangs(const QString& sentence, QString& country1, QString& country2)
{
    country1.clear();
    country2.clear();
    QStringList listCountry = _countryCode.keys();
    for(QString country : listCountry)
    {
       QJsonObject object =  _countryCode[country].toObject();
       int index = sentence.indexOf(object["code"].toString());
        if(index  == 0)
        {
            country1 = country;
        }
        else if(index > 0)
        {
            country2 = country;
        }
    }
    if(country1.isEmpty() || country2.isEmpty()) setErrorLabel(_errors["ERR_UNKNOWN_LANG"].toString());
    if(country1 == country2) setErrorLabel(_errors["ERR_LANG_DB"].toString());
    return  !country1.isEmpty() && !country2.isEmpty() && country1 != country2;
}

void VideoPlayer::srtChanged(const QString& srtPath)
{
    _readSrt = QSharedPointer<ReadSrt>(new ReadSrt(srtPath));
}

ReadSrt::subId VideoPlayer::findSubId(const qint64& position)
{
    ReadSrt::subId id = -1;
    if(_readSrt == nullptr)return id;
    QMap<qint64, qint64> subDuration(_readSrt->getSubDuration());
    QMapIterator<ReadSrt::subId, qint64> subPositionI(_readSrt->getSubPosition());

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

QLabel* VideoPlayer::findTranslation(const QString& sentence, const ReadSrt::subId& id)
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

void VideoPlayer::updateSubtitle(qint64 position)
{
    static ReadSrt::subId currentId =  -1;
    static ReadSrt::subId oldId =  -1;

    currentId = findSubId(position);
    if(currentId == -1)
    {
        emit clearLabelLayout();
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

            QLabel * translation = findTranslation(sentence, currentId);

            if(count - currentPosition == 1 || translation)
            {
                currentSub.append(translation ? translation : new QLabel(sentence));
                currentPosition = count;
                count = subSplit.size() + 1;
            }

        }while(currentPosition < subSplit.size());

        emit clearLabelLayout();

        for(int i = 0; i < currentSub.size(); i++)
            emit addNewLabel(currentSub[i]);
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    updateSubtitle(position);
}

void VideoPlayer::closed()
{
    _window->close();
}

void VideoPlayer::enterSub()
{
    emit pauseVideoPlayer();
}

void VideoPlayer::newDownloadWidget(DownloadWidget* d)
{
    connect(d, SIGNAL(srtChanged(const QString&)), this, SLOT(srtChanged(const QString&)));
}

void VideoPlayer::openWebSite()
{
    if(_mediaInfo.path().isEmpty())
        return;

    if(!_browser || _browser->isMainWinDestroyed())
    {
        try
        {
            emit pauseVideoPlayer();
            _browser = new Browser(_mediaInfo);
            _window = _browser->createWindow();

            QString mediaName = _mediaInfo.fileName().replace("."+_mediaInfo.suffix(),"");
            _window->setUrl(QUrl("https://www.opensubtitles.org/en/search2/sublanguageid-"
                + _lang1 + "/moviename-" + mediaName.replace("-"+_lang1 + _lang2,"")));
            connect(_browser->downloadManagerWidget(), SIGNAL(newDownloadWidget(DownloadWidget*)),
                this, SLOT(newDownloadWidget(DownloadWidget*)));
        }catch(MediaNotFoundException& ex)
        {
           setErrorLabel(ex.getMessage());
        }
    }
}


