#include "slider.h"

#include <QDebug>

Slider::Slider(QWidget *widget)
    : QSlider(Qt::Orientation::Horizontal, widget)
{
}

void Slider::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (orientation() == Qt::Horizontal)
        {
            qint64 position = minimum() + (maximum() - minimum())/width() * (qint64)event->x();
            emit positionChanged(position);
        }
        event->accept();
    }
    QSlider::mousePressEvent(event);
}
