#include "game.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>

Game::Game(QWidget *parent) : QWidget(parent)
{
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);
    hintButton = new Lbutton(this, "提示");
    hintButton->move(1400, 200);
    withdrawButton = new Lbutton(this, "撤销");
    withdrawButton->move(1400, 400);
    pathToggleButton = new Lbutton(this, "显示路径");
    pathToggleButton->move(1400, 600);
}

Game::~Game()
{
}

void Game::setMap(MapData mapData)
{
    hexagons = mapData.hexagons;
    radius = mapData.radius;
    color1 = mapData.color1;
    color2 = mapData.color2;
    center = mapData.center;
    color3 = mapData.color3;
    path = mapData.path;
}

// 绘制所有六边形
void Game::paintEvent(QPaintEvent *event)
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

void Game::mousePressEvent(QMouseEvent *event)
{
    QPointF click = event->pos();
    double mn = 1E9;
    for (HexCell &hex : hexagons)
    {
        mn = fmin(mn, QLineF(click, hex.center).length());
    }
    for (HexCell &hex : hexagons)
    {
        if (QLineF(click, hex.center).length() == mn && mn <= radius)
        {
            hex.color = (hex.color == color1) ? color2 : color1;
            break; // 只翻转一个
        }
    }
    update();
}

void Game::drawHexagon(QPainter &painter, const QPointF &center, int radius)
{
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i)
    {
        double angle = M_PI / 3 * i;
        double x = center.x() + radius * cos(angle);
        double y = center.y() + radius * sin(angle);
        hexagon << QPointF(x, y);
    }
    painter.drawPolygon(hexagon);
}
