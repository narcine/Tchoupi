#ifndef READSRT_H
#define READSRT_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QStringList>

/**
 * @brief This class reads the srt files
 */
class ReadSrt
{
public:
    using subId = qint64;
    ReadSrt(const QString& filename); /**< Read the file */

    /**
     * @brief Get the subtitles
     * @return The subitles
     */
    const QMap<subId, QString>& getSub();

    /**
     * @brief Get the subtitles duration
     * @return The subtitles duration
     */
    const QMap<qint64,qint64>& getSubDuration();

    /**
     * @brief Get the subtitles starting position
     * @return The subtitles starting position
     */
    const QMap<subId, qint64>& getSubPosition();

private :
    QTextStream _flux; /**< The file stream */
    QMap<subId, QString> _mapSub; /**< The subtitles  */
    QMap<qint64, qint64> _mapDuration; /**< The subtitles duration */
    QMap<subId, qint64> _mapSubPosition; /**< The subtitles starting position */

    /**
     * @brief Read the text inside the file
     */
    void readText();

    /**
     * @brief Know if it is the section end
     * @param line The line to check
     * @return true if the end is reached otherwise false
     */
    bool isTheEnd(const QString& line);

    /**
     * @brief Know if it is a new section
     * @param line The line to check
     * @return true if a new section is found otherwise false
     */
    bool isNewSub(const QString& line);

    /**
     * @brief Get the subtitle number
     * @param line The line to check
     * @return The subtitle number or zero
     */
    subId getSubId(const QString& line);

    /**
     * @brief convertStringToDuration
     * @param duration The subtitle duration
     * @return the conversion result
     */
    qint64 convertStringToDuration(const QString& duration);

    /**
     * @brief Add a subtitle in the map
     * @param id The subtitle number to add
     */
    void addSub(const subId& id);

    /**
     * @brief Add a subtitle duration in the map
     * @param duration The subtitle duration to add
     * @param id The subtitle number related to the subtitle duration
     */
    void addDuration(const QString& duration, const subId& id);

    /**
     * @brief Add a subtitle
     * @param subtitle The subtitle to add
     * @param id The subtitle number related to the subtitle
     */
    void addSubtitle(const QString& subtitle, const subId& id);

    /**
     * @brief Remove unecessary characters
     * @param line The line to check
     */
    void purgeLine(QString& line);
};

#endif // READSRT_H
