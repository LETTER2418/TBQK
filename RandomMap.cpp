#include "RandomMap.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib> // for rand()

RandomMap::RandomMap(QWidget *parent) : QWidget(parent)
{
    backButton = new Lbutton("返回", this);
    backButton->move(0, 0);

    generateHexagons(); // 初始化一次地图
}

RandomMap::~RandomMap()
{
}

void RandomMap::generateHexagons() {
    hexagons.clear();

    const QVector<QPoint> directions = {
        {1, 0}, {1, -1}, {0, -1},
        {-1, 0}, {-1, 1}, {0, 1}
    };

    auto hexToPixel = [=](int q, int r) -> QPointF {
        double x = radius * 3.0 / 2 * q;
        double y = radius * std::sqrt(3) * (r + q / 2.0);
        return center + QPointF(x, y);
    };

    QSet<QPoint> visited;
    QVector<QPoint> path;

    QPoint current(0, 0);
    path.append(current);
    visited.insert(current);


    int maxLength = 3 * rings * (rings + 1); // 允许较长路径，防止过短

    while (path.size() < maxLength) {
        QVector<QPoint> candidates;
        for (const QPoint &dir : directions) {
            QPoint next = current + dir;
            if (!visited.contains(next)) {
                candidates.append(next);
            }
        }

        if (candidates.isEmpty()) {
            break; // 无路可走
        }

        QPoint next = candidates[rand() % candidates.size()];
        current = next;
        path.append(current);
        visited.insert(current);
    }

    // 将路径中的点变成黑色，其余填成白色（可选）
    QSet<QPoint> pathSet = visited;

    for (int q = -rings; q <= rings; ++q) {
        for (int r = -rings; r <= rings; ++r) {
            if (std::abs(q + r) > rings) continue;
            QPoint pos(q, r);
            QPointF pixel = hexToPixel(q, r);
            QColor color = pathSet.contains(pos) ? Qt::black : Qt::white;
            hexagons.push_back({pixel, color});
        }
    }
}


void RandomMap::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::gray);

    for (const HexCell &hex : hexagons) {
        painter.setBrush(hex.color);
        drawHexagon(painter, hex.center, radius);
    }
}

void RandomMap::mousePressEvent(QMouseEvent *event)
{
    QPointF click = event->pos();

    for (HexCell &hex : hexagons) {
        if (QLineF(click, hex.center).length() <= radius) {
            hex.color = (hex.color == Qt::black) ? Qt::white : Qt::black;
            break; // 只翻转一个
        }
    }

    update();
}

void RandomMap::drawHexagon(QPainter &painter, const QPointF &center, int radius)
{
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3 * i;
        double x = center.x() + radius * std::cos(angle);
        double y = center.y() + radius * std::sin(angle);
        hexagon << QPointF(x, y);
    }
    painter.drawPolygon(hexagon);
}
