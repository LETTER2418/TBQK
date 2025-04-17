#ifndef RANDOMMAP_H
#define RANDOMMAP_H
#include <QWidget>
#include "Lbutton.h"
#include "MessageBox.h"
struct HexCell {
    QPointF center;
    QColor color;
};
class RandomMap: public QWidget
{
    Q_OBJECT

public:
    RandomMap(QWidget *parent = nullptr);
    ~RandomMap();
    Lbutton *backButton;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);
    void generateHexagons(); // 新增：初始化六边形

private:
    MessageBox *messageBox;
    QVector<HexCell> hexagons;
    int radius = 50;
    int rings = 3;
    QPointF center = QPointF(850, 440); // 地图中心坐标

};

#endif // RANDOMMAP_H
