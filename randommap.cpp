#include "randommap.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>

RandomMap::RandomMap(QWidget *parent) : QWidget(parent)
{
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);
    saveButton = new Lbutton(this, "保存");
    saveButton->move(1300, 400);
    id = 0;
}

RandomMap::~RandomMap()
{
}

// 生成六边形网格地图
void RandomMap::generateHexagons(int rings, QColor c1, QColor c2, QColor c3)
{
    hexagons.clear();
    path.clear();

    color1 = c1;
    color2 = c2;
    color3 = c3;

    const QVector<QPoint> directions =
    {
        {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}
    };

    auto hexToPixel = [ = ](int q, int r) -> QPointF
    {
        double x = radius * 3.0 / 2 * q;
        double y = radius * std::sqrt(3) * (r + q / 2.0);
        return center + QPointF(x, y);
    };

    QSet<QPoint> visited;
    QPoint current(0, 0);
    path.append(current);
    visited.insert(current);

    int maxLength = 3 * rings * (rings + 1);

    while (path.size() < maxLength)
        {
            QVector<QPoint> candidates;
            for (const QPoint &dir : directions)
                {
                    QPoint next = current + dir;
                    auto [q, r] = next;
                    if (abs(q + r) <= rings && abs(q) <= rings && abs(r) <= rings && !visited.contains(next))
                        {
                            candidates.append(next);
                        }
                }

            if (candidates.isEmpty())
                {
                    break; // 无路可走
                }

            // 随机的挑选一个可访问的六边形
            QPoint next = candidates[rand() % candidates.size()];
            current = next;
            path.append(current);
            visited.insert(current);
        }

    QSet<QPoint> pathSet = visited;

     // 为每个环生成翻转状态
    int f = rand() % 2;
    QVector<bool> ringFlipStatus(rings + 1);
    for (int ring = 0; ring <= rings; ++ring)
        {
            ringFlipStatus[ring] = ++f % 2 == 0; // 决定是否翻转
        }

    for (int q = -rings; q <= rings; ++q)
        {
            for (int r = -rings; r <= rings; ++r)
                {
                    if (std::abs(q + r) > rings)
                        {
                            continue;
                        }
                    QPoint pos(q, r);
                    QPointF pixel = hexToPixel(q, r);

                    // 计算当前六边形所在的环
                    int ring = (std::abs(q) + std::abs(r) + std::abs(q + r)) / 2;

                    // 获取环的翻转状态
                    bool flip = ringFlipStatus[ring];

                    // 根据翻转状态决定颜色
                    bool isPath = pathSet.contains(pos);
                    QColor color;
                    if (flip)
                        {
                            color = isPath ? c2 : c1;
                        }
                    else
                        {
                            color = isPath ? c1 : c2;
                        }

                    hexagons.push_back({pixel, color});
                }
        }
}

// 绘制所有六边形
void RandomMap::paintEvent(QPaintEvent *event)
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

void RandomMap::drawHexagon(QPainter &painter, const QPointF &center, int radius)
{
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i)
        {
            double angle = M_PI / 3 * i;
            double x = center.x() + radius * std::cos(angle);
            double y = center.y() + radius * std::sin(angle);
            hexagon << QPointF(x, y);
        }
    painter.drawPolygon(hexagon);
}

MapData RandomMap::getMapData()
{
    MapData data;
    data.hexagons = this->hexagons; // 复制六边形单元格数据
    data.radius = this->radius;     // 复制六边形半径
    data.center = this->center;     // 复制地图中心坐标
    data.color1 = this->color1;     // 复制第一种颜色
    data.color2 = this->color2;     // 复制第二种颜色
    data.color3 = this->color3;     // 复制第三种颜色
    data.id = this->id;             // 复制关卡索引
    data.path = this->path;         // 复制提示路径
    data.generation = "random";     // 设置为随机生成
    return data;
}

void RandomMap::setId(int id_)
{
    id = id_;
}
