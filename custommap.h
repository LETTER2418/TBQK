#ifndef CUSTOMMAP_H
#define CUSTOMMAP_H

#include "lbutton.h"
#include "mapmanager.h"
#include "messagebox.h"
#include <QWidget>

class CustomMap : public QWidget
{
    Q_OBJECT

public:
    CustomMap(QWidget *parent = nullptr);
    ~CustomMap();
    Lbutton *backButton;
    Lbutton *saveButton;
    void generateHexagons(int rings = 3, QColor c1 = Qt::black, QColor c2 = Qt::white);
    MapData getMapData();
    void setId(int id_);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);

private:
    MessageBox *messageBox;
    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440);
    QColor color1, color2;
    int id;
};

#endif // CUSTOMMAP_H
