#ifndef RANDOMMAP_H
#define RANDOMMAP_H
#include <QWidget>
#include "BUTTON.h"

class RandomMap: public QWidget
{
    Q_OBJECT

public:

    RandomMap(QWidget *parent = nullptr);
    ~RandomMap();

    BUTTON *backButton;

protected:
    void paintEvent(QPaintEvent *event) override ;

private:
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);

};

#endif // RANDOMMAP_H
