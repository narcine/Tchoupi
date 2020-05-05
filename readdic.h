#ifndef READDIC_H
#define READDIC_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QStringList>

class ReadDic
{
public:
    ReadDic(QString);
    ~ReadDic();
    const QMap<QString, QMap<QString, QString>>& getDictionary();

private :
    QTextStream _flux;
    QMap<QString, QMap<QString, QString>> _mapDictionary;
    QFile _fichier;

    void readText();
    bool isTheEnd(QString);
    bool isNewWords(QString);
    QString getWords(QString);
    void addWords(QString);
    void addTraduction(QString, QString);
};

#endif // READDIC_H
