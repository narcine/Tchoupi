#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QMouseEvent>

class Slider: public QSlider
{
    Q_OBJECT
public:
    Slider(QWidget* widget = nullptr);
    void mousePressEvent (QMouseEvent* event);
signals:
    void positionChanged(qint64 position);
};

#endif // SLIDER_H
