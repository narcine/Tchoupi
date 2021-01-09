#ifndef LABELLAYOUT_H
#define LABELLAYOUT_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QString>
#include <QMediaPlayer>
#include <QMutex>

/**
 * @brief This class add a translation to a label
 */
class SubLabel : public QLabel
{
     Q_OBJECT
public:
    SubLabel(const QString& text); /**< The text to display on the screen */

signals :
    void enter(); /**< The signal is emitted when the event Qevent::Enter is received */

protected:
    bool event(QEvent *event) override;
};

#endif // LABELLAYOUT_H
