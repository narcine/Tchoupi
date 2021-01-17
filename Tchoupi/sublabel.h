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

private:
    bool event(QEvent *event) override; /**< Color the label when the mouse moves over */

signals:
    void enter(); /**< When the mouse moves over the label */
    void leave(); /**< When the mouse leaves the label */
};

#endif // LABELLAYOUT_H
