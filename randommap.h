#ifndef RANDOMMAP_H
#define RANDOMMAP_H
#include <QWidget>
#include "lbutton.h"
#include "messagebox.h"
#include "mapmanager.h"

class RandomMap: public QWidget
{
    Q_OBJECT

public:
    RandomMap(QWidget *parent = nullptr);
    ~RandomMap();
    Lbutton *backButton;
    Lbutton *saveButton;
    void generateHexagons(int rings = 3, QColor c1 = Qt::black, QColor c2 = Qt::white, QColor c3 = Qt::green); // 生成六边形
    MapData getMapData();
    void setId(int id_);

protected:
    void paintEvent(QPaintEvent *event) override;
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);


private:
    QVector<QPoint>path;
    MessageBox *messageBox;
    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440); // 地图中心坐标
    QColor color1, color2, color3;
    int id;
};

#endif // RANDOMMAP_H
