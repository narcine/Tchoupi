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

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QSharedPointer>
#include <QMediaPlayer>
#include <QWidget>
#include <QJsonDocument>
#include <QFileInfo>
#include <QJsonObject>

#include "readdic.h"
#include "readsrt.h"
#include "sublabel.h"
#include "videoplayergui.h"
#include "browser.h"
#include "browserwindow.h"

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
class QBoxLayout;
QT_END_NAMESPACE

/**
 * @brief This class handles the player
 */
class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer() override;
    void openFile();
    void positionChanged(qint64 position);
    void openWebSite();

private slots:
    void enterSub();
    void closed();
    void srtChanged(const QString& filePath);
    void newDownloadWidget(DownloadWidget* d);

signals:
    void setErrorLabel(const QString&);
    void pauseVideoPlayer();
    void addNewLabel(QLabel*);
    void clearLabelLayout();
    void setPositionValue(qint64);
    void setMedia(const QUrl&);
    void setIcon(const QString&, const QString&);

private:
    QSharedPointer<ReadDic> _readDic;
    QSharedPointer<ReadSrt> _readSrt;
    QSharedPointer<ReadDic> _readWords;
    BrowserWindow* _window;
    Browser* _browser;
    QFileInfo _mediaInfo;
    QString _lang1;
    QString _lang2;
    QJsonObject _countryCode;
    QJsonObject _errors;

    /**
     * @brief checkLangs Check if the sentence contains both country codes
     * @param sentence The sentence to check
     * @param country1 The first country code corresponding to the first language
     * @param country2 The second country code corresponding to the second language
     * @return true if found, otherwise false.
     */
    bool checkLangs(const QString& sentence, QString& country1, QString& country2);


    /**
     * @brief updateSubtitle Update the subtitle depending on the position of the content
     * @param position The position of the content
     */
    void updateSubtitle(qint64 position);

    /**
     * @brief Find the appropriated translation
     * @param sentence The sentence to check
     * @param id Use the ID to look for a translation related to the sentence
     * @return The desired translation
     */
    QLabel* findTranslation(const QString& sentence, const ReadSrt::subId& id = -1);

    /**
     * @brief Clear the layout
     * @param layout The layout to clear
     */
    void clearLayout(QLayout *layout);

    /**
     * @brief Find the subtitle number
     * @param position The position to retrieve the number
     * @return the subtitle number
     */
    ReadSrt::subId findSubId(const qint64& position);

    /**
     * @brief The url that will be chosen to open the video
     * @param url The url to open
     */
    void setUrl(const QUrl &url);

    /**
     * @brief Create a new subtitle label
     * @param text The text to display on the screen
     * @param tooltip The tooltip text
     * @return The new subtitle label
     */
    SubLabel* createSubLabel(const QString& text, const QString& tooltip = "");
};

#endif // VIDEOPLAYER_H
