#include "readdic.h"

ReadDic::ReadDic(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    _flux.setDevice(&file);
    readText();
}

const QMap<QString,QMap<QString,QString>>& ReadDic::getDictionary()
{
    return _mapDictionary;
}

void ReadDic::readText()
{
    QString line;
    while(! _flux.atEnd())
    {
        line = _flux.readLine();
        QString words = getWords(line);
        if(!words.isEmpty())
        {
            addWords(words);
            while(!isTheEnd(line))
            {
                line = _flux.readLine();
                if(isNewWords(line))
                {
                    break;
                }
                addTranslation(line, words);
            }
        }
    }
}

bool ReadDic::isTheEnd(const QString& line)
{
    return line.trimmed().isEmpty();
}

bool ReadDic::isNewWords(const QString& line)
{
    return line.contains(":");
}

QString ReadDic::getWords(const QString& line)
{
    QString newWords {};
    if(isNewWords(line))
    {
        QStringList list = line.split(":");
        newWords = list[0].trimmed();
    }
    return newWords;
}

void ReadDic::addWords(const QString& words)
{
    _mapDictionary.insert(words, QMap<QString, QString>());
}

void ReadDic::addTranslation(const QString& translation, const QString& words)
{
    if(translation.isEmpty() || translation.split(",").count() != 2)
        return;

    if(_mapDictionary.contains(words))
    {
        QStringList list = translation.split(",");
        QString numSubtitle = list[0].trimmed();
        QString traducForSub = list[1].trimmed();

        QMap<QString,QString>& mapTraductions = _mapDictionary[words];
        mapTraductions.insert(numSubtitle, traducForSub);
   }
}
