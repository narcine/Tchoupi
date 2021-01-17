#include "readsrt.h"

#include <QDebug>

ReadSrt::ReadSrt(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    _flux.setDevice(&file);
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
    QString line;
    while(!_flux.atEnd())
    {
        line = _flux.readLine();
        if(isNewSub(line))
        {
            subId subId = getSubId(line);
            line = _flux.readLine();
            addDuration(line, subId);
            addSub(subId);
            while(!isTheEnd(line))
            {
                line = _flux.readLine();
                purgeLine(line);
                if(isNewSub(line))
                {
                    break;
                }
                addSubtitle(line, subId);
            }
        }
    }
}

void ReadSrt::purgeLine(QString& line)
{
    //remove special characters and trailing spaces
    line.replace(QRegExp("<[^<>]+>"),"");
    QStringList characters = { "?", "_", "\"", ",", "#", ".", "^", "*", "~", "&", "|", "\\", "/",
        "!", "@", "=", "+", "$", "-", ":", "'", "[", "]", "(", ")", "{", "}"};
    for(QString c : characters)
    {
        line.replace(c, " " + c + " ");
    }
    line.replace(QRegExp("\\s{2,}")," ");
    line = line.trimmed();
}

bool ReadSrt::isTheEnd(const QString& line)
{
    return line.trimmed().isEmpty();
}

bool ReadSrt::isNewSub(const QString& line)
{
    bool ok  = false;
    line.toInt(&ok);
    return ok;
}

ReadSrt::subId ReadSrt::getSubId(const QString& line)
{
    subId newSubId = 0;
    if(isNewSub(line)){
       newSubId = line.toLongLong();
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
