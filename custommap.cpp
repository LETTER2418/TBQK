#include "custommap.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>

CustomMap::CustomMap(QWidget *parent) : QWidget(parent)
{
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);
    saveButton = new Lbutton(this, "保存");
    saveButton->move(1300, 400);
    id = 0;
}

CustomMap::~CustomMap()
{
}

// 生成六边形网格地图
void CustomMap::generateHexagons(int rings, QColor c1, QColor c2, QColor c3)
{
    hexagons.clear();
    color1 = c1;
    color2 = c2;
    color3 = c3;

    //轴向坐标转成笛卡尔坐标
    auto hexToPixel = [ = ](int q, int r) -> QPointF
    {
        double x = radius * 3.0 / 2 * q;
        double y = radius *  sqrt(3) * (r + q / 2.0);
        return center + QPointF(x, y);
    };

    for (int q = -rings; q <= rings; ++q)
        {
            for (int r = -rings; r <= rings; ++r)
                {
                    if (abs(q + r) > rings)
                        {
                            continue;
                        }
                    QPoint pos(q, r);
                    QPointF pixel = hexToPixel(q, r);
                    hexagons.push_back({pixel, c1});
                }
        }
}

// 绘制所有六边形
void CustomMap::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::gray);
    for (const HexCell &hex : hexagons)
        {
            painter.setBrush(hex.color);
            drawHexagon(painter, hex.center, radius);
        }
}

void CustomMap::mousePressEvent(QMouseEvent *event)
{
    QPointF click = event->pos();
    double mn = 1E9;
    for (HexCell &hex : hexagons)
        {
            mn = fmin(mn, QLineF(click, hex.center).length());
        }
    for (HexCell &hex : hexagons)
        {
            if (QLineF(click, hex.center).length() == mn)
                {
                    hex.color = (hex.color == color1) ? color2 : color1;
                    break; // 只翻转一个
                }
        }
    update();
}

void CustomMap::drawHexagon(QPainter &painter, const QPointF &center, int radius)
{
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i)
        {
            double angle = M_PI / 3 * i;
            double x = center.x() + radius *  cos(angle);
            double y = center.y() + radius *  sin(angle);
            hexagon << QPointF(x, y);
        }
    painter.drawPolygon(hexagon);
}

MapData CustomMap::getMapData()
{
    MapData data;
    data.hexagons = this->hexagons;    // 复制六边形单元格数据
    data.radius = this->radius;       // 复制六边形半径
    data.center = this->center;       // 复制地图中心坐标
    data.color1 = this->color1;       // 复制第一种颜色
    data.color2 = this->color2;       // 复制第二种颜色
    data.color3 = this->color3;       // 复制第三种颜色
    data.id = this->id;               // 复制关卡索引
    return data;
}

void CustomMap::setId(int id_)
{
    id = id_;
}
