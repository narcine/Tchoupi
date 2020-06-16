#ifndef READDIC_H
#define READDIC_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QStringList>

/**
 * @brief This class reads the dic files
 */
class ReadDic
{
public:
    ReadDic(QString); /**< Read the file */

    /**
     * @brief Get the translations dictionary
     * @return the actual translations dictionary
     */
    const QMap<QString, QMap<QString, QString>>& getDictionary();

private :
    QTextStream _flux; /**< The file stream */
    QMap<QString, QMap<QString, QString>> _mapDictionary; /**< The translations dictionary */

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
    bool isNewWords(const QString& line);

    /**
     * @brief Obtain the group of words
     * @param line The line to check
     * @return the group of words or an empty string
     */
    QString getWords(const QString& line);

    /**
     * @brief Add a group of words in the map
     * @param words The group of words to add in the map
     */
    void addWords(const QString& words);

    /**
     * @brief Add a new translation in the map
     * @param translation The translation of the group of words
     * @param words The group of words related to the translation
     */
    void addTranslation(const QString& translation, const QString& words);
};

#endif // READDIC_H
