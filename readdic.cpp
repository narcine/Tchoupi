#include "readdic.h"

#include <QDebug>

ReadDic::ReadDic(QString filename)
{
    _fichier.setFileName(filename);
    _fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    _flux.setDevice(&_fichier);
    readText();
}

const QMap<QString,QMap<QString,QString>>& ReadDic::getDictionary()
{
    return _mapDictionary;
}

void ReadDic::readText()
{
    QString ligne;
    while(! _flux.atEnd())
    {
        ligne = _flux.readLine();
        QString words = getWords(ligne);
        if(!words.isEmpty())
        {
            addWords(words);
            while(!isTheEnd(ligne))
            {
                ligne = _flux.readLine();
                if(isNewWords(ligne))
                {
                    break;
                }
                addTraduction(ligne, words);
            }
        }
    }
}

bool ReadDic::isTheEnd(QString ligne)
{
    return ligne.trimmed().isEmpty();
}

bool ReadDic::isNewWords(QString ligne)
{
    return ligne.contains(":");
}

QString ReadDic::getWords(QString ligne)
{
    QString newWords {};
    if(isNewWords(ligne))
    {
        QStringList list = ligne.split(":");
        newWords = list[0].trimmed();
    }
    return newWords;
}

void ReadDic::addWords(QString word)
{
    _mapDictionary.insert(word, QMap<QString, QString>());
}

void ReadDic::addTraduction(QString traduction, QString words)
{
    if(traduction.isEmpty() || traduction.split(",").count() != 2)
        return;

    if(_mapDictionary.contains(words))
    {
        QStringList list = traduction.split(",");
        QString numSubtitle = list[0].trimmed();
        QString traducForSub = list[1].trimmed();

        QMap<QString,QString>& mapTraductions = _mapDictionary[words];
        mapTraductions.insert(numSubtitle, traducForSub);
   }
}
