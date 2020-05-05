#ifndef LABELLAYOUT_H
#define LABELLAYOUT_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QString>
#include <QMediaPlayer>
#include <QMutex>

class SubLabel : public QLabel
{
     Q_OBJECT
public:
    SubLabel(QWidget *parent = nullptr);
    SubLabel(const QString&);

signals :
    void enter();

protected:
    bool event(QEvent *event) override;
};

#endif // LABELLAYOUT_H
