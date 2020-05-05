#include "readsrt.h"

#include <QDebug>

ReadSrt::ReadSrt(QString filename)
{
    _fichier.setFileName(filename);
    _fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    _flux.setDevice(&_fichier);
    readText();
}

const QMap<ReadSrt::subId, QString>& ReadSrt::getSub()
{
    return _mapSub;
}

const QMap<qint64, qint64>& ReadSrt::getSubDuration()
{
    return _mapDuration;
}

const QMap<ReadSrt::subId, qint64>& ReadSrt::getSubPosition()
{
    return _mapSubPosition;
}

void ReadSrt::readText()
{
    QString ligne;
    while(!_flux.atEnd())
    {
        ligne = _flux.readLine();
        if(isNewSub(ligne))
        {
            subId subId = getSubId(ligne);
            ligne = _flux.readLine();
            addDuration(ligne, subId);
            addSub(subId);
            while(!isTheEnd(ligne))
            {
                ligne = _flux.readLine();
                purgeLine(ligne);
                if(isNewSub(ligne))
                {
                    break;
                }
                addSubtitle(ligne, subId);
            }
        }
    }
}

void ReadSrt::purgeLine(QString& line)
{
    //remove special characters and trailing spaces
    line.replace(QRegExp("<[^<>]+>"),"");
    line.replace("?"," ? ");
    line.replace("_"," _ ");
    line.replace("\""," \" ");
    line.replace(","," , ");
    line.replace("#"," # ");
    line.replace("."," . ");
    line.replace("^"," ^ ");
    line.replace("*"," * ");
    line.replace("~"," ~ ");
    line.replace("&"," & ");
    line.replace("|"," | ");
    line.replace("\\"," \\ ");
    line.replace("/"," / ");
    line.replace("!"," ! ");
    line.replace("@"," @ ");
    line.replace("#"," # ");
    line.replace("="," = ");
    line.replace("+"," + ");
    line.replace("$"," $ ");
    line.replace("-"," - ");
    line.replace(":"," : ");
    line.replace("["," [ ");
    line.replace("]"," ] ");
    line.replace("("," ( ");
    line.replace(")"," ) ");
    line.replace("{"," { ");
    line.replace("}"," } ");
    line.replace("\\s{2,}"," ");
    line = line.trimmed();
}

bool ReadSrt::isTheEnd(const QString& ligne)
{
    return ligne.trimmed().isEmpty();
}

bool ReadSrt::isNewSub(const QString& ligne)
{
    bool ok  = false;
    ligne.toInt(&ok);
    return ok;
}

ReadSrt::subId ReadSrt::getSubId(const QString& ligne)
{
    subId newSubId = 0;
    if(isNewSub(ligne)){
       newSubId = ligne.toLongLong();
    }
    return newSubId;
}

qint64 ReadSrt::convertStringToDuration(const QString& duration)
{
  //example : 00:20:50,526
  QStringList durFormat = duration.trimmed().split(":");
  qint64 time = static_cast<qint64>(durFormat[0].toFloat()*60.0f*60.0f*1000.0f +
          durFormat[1].toFloat()*60.0f*1000.0f + durFormat[2].replace(',','.').toFloat()*1000.0f);
  return time;
}

void ReadSrt::addSub(const subId& id)
{
    _mapSub.insert(id, QString());
}

void ReadSrt::addDuration(const QString& duration, const subId& id)
{
    QStringList list = duration.split("-->");
    _mapDuration.insert(convertStringToDuration(list[0]), convertStringToDuration(list[1]));
    _mapSubPosition.insert(id, convertStringToDuration(list[0]));
}

void ReadSrt::addSubtitle(const QString& subtitle, const subId& id)
{
    if(subtitle.isEmpty())
        return;

    if(_mapSub.contains(id))
    {
        QString newSub = _mapSub[id];
        newSub = newSub + " " + subtitle;
        _mapSub.insert(id, newSub.trimmed());
    }
}
