#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QMouseEvent>

/**
 * @brief This class custumize the slider in order to jump a desired position
 */
class Slider: public QSlider
{
    Q_OBJECT
public:
    Slider(QWidget* widget = nullptr);
    void mousePressEvent (QMouseEvent* event) override; /**< Use the mouse to jump */
signals:
    void positionChanged(qint64 position); /**< Change the position of the media */
};

#endif // SLIDER_H
