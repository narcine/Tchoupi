#ifndef READSRT_H
#define READSRT_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QStringList>

class ReadSrt
{
public:
    using subId = qint64;
    ReadSrt(const ReadSrt &);
    ReadSrt(QString);
    ~ReadSrt();
    const QMap<subId, QString>& getSub();
    const QMap<qint64,qint64>& getSubDuration();
    const QMap<subId, qint64>& getSubPosition();

private :
    QTextStream _flux;
    QFile _fichier;
    QMap<subId, QString> _mapSub;
    QMap<qint64, qint64> _mapDuration;
    QMap<subId, qint64> _mapSubPosition;

    void readText();
    bool isTheEnd(const QString&);
    bool isNewSub(const QString&);
    subId getSubId(const QString&);
    qint64 convertStringToDuration(const QString&);
    void addSub(const subId&);
    void addDuration(const QString&, const subId&);
    void addSubtitle(const QString&, const subId&);
    void purgeLine(QString&);
};

#endif // READSRT_H
