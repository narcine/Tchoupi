#include "sublabel.h"

#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>

SubLabel::SubLabel(const QString& text)
    : QLabel(text)
{
}

bool SubLabel::event(QEvent *event)
{
    switch(event->type())
    {
        case QEvent::Enter :
            this->setStyleSheet("QLabel { color : blue }");
            emit enter();
        break;
        case QEvent::Leave :
            this->setStyleSheet("QLabel { color : black }");
        break;
        case QEvent::ToolTip :
        {
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
            QToolTip::showText(helpEvent->globalPos(),toolTip());
        }
        break;
        default :
            QToolTip::hideText();
            event->ignore();
        break;
    }
    return QLabel::event(event);
}
