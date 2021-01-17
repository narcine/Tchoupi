#ifndef VIDEOPLAYERGUI_H
#define VIDEOPLAYERGUI_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QLabel>
#include <QGraphicsView>
#include <QFileDialog>
#include <QJsonObject>

#include "errors.h"
#include "readdic.h"
#include "readsrt.h"
#include "sublabel.h"

namespace Ui {
class Tchoupi;
}

class Tchoupi : public QWidget
{
    Q_OBJECT

public:
    explicit Tchoupi(QWidget *parent = nullptr);
    ~Tchoupi();

private:
    QString _totalDuration;
    QJsonObject _countryCodes;
    Ui::Tchoupi* _ui;
    std::unique_ptr<QMediaPlayer> _mediaPlayer;
    std::unique_ptr<ReadDic> _readDic;
    std::unique_ptr<ReadSrt> _readSrt;
    std::unique_ptr<ReadDic> _readWords;

    void keyPressEvent(QKeyEvent *) override;

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
    ReadSrt::subId findSubId(ReadSrt* const readSrt, const qint64& position);

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

    /**
     * @brief Read the video
     * @param url the path of the video
     */
    void setMedia(const QUrl& url);

    /**
     * @brief Set a new icon with the image of the country codes
     * @param country1 Path of the first country
     * @param country2 Path of the second country
     */
    void setIcon(const QString& country1, const QString& country2);

    /**
     * @brief Display a messsage according to the error code
     * @param err The error code
     */
    void setErrorLabel(Errors err);

    /**
     * @brief Add a new label to the label layout
     * @param label The label to add
     */
    void addNewLabel(QLabel* label);

    /**
     * @brief Clear the label layout
     */
    void clearLabelLayout();

private slots:
    void mediaStateChanged(QMediaPlayer::State state); /**< Play or pause according to media state */
    void durationChanged(qint64 duration); /**< Update the duration of the media */
    void handleError(); /**< Display a blocking error */
    void openFile(); /**< Open a file */
    void setPosition(qint64 position); /**< Set the media's position */
    void positionChanged(qint64 position); /**< Update the slider's position and subtitle */
    void handlePlayButton(); /**< Play or pause according to play button */
};

#endif // VIDEOPLAYERGUI_H
