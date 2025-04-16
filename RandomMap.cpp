#include "RandomMap.h"
#include <QBoxLayout>

RandomMap::RandomMap(QWidget *parent):QWidget(parent)
{
    backButton = new BUTTON("返回", this);
    backButton -> move(0, 0);

}

void RandomMap::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int radius = 30;  // 六边形的半径
    int rings = 3;    // 同心环数，包含中心（0层）

    // 中心坐标（屏幕坐标）
    QPointF center(400, 300);

    // 方向向量（轴向坐标系）
    const QVector<QPoint> directions = {
        {1, 0}, {1, -1}, {0, -1},
        {-1, 0}, {-1, 1}, {0, 1}
    };

    // lambda函数：将轴向坐标转换为像素坐标
    auto hexToPixel = [=](int q, int r) -> QPointF {
        double x = radius * 3.0 / 2 * q;
        double y = radius * std::sqrt(3) * (r + q / 2.0);
        return center + QPointF(x, y);
    };

    // 遍历所有环
    for (int k = 0; k <= rings; ++k) {
        if (k == 0) {
            // 中心六边形
            QColor color = (rand() % 2 == 0) ? Qt::black : Qt::white;
            painter.setBrush(color);
            painter.setPen(Qt::gray);
            drawHexagon(painter, center, radius);
        } else {
            // 每个环从六个方向中一个起点开始，然后顺时针绕一圈
            QPoint q = directions[4] * k;  // 起点
            int cur_q = q.x();
            int cur_r = q.y();

            for (int dir = 0; dir < 6; ++dir) {
                for (int step = 0; step < k; ++step) {
                    QPointF pos = hexToPixel(cur_q, cur_r);
                    QColor color = (rand() % 2 == 0) ? Qt::black : Qt::white;
                    painter.setBrush(color);
                    painter.setPen(Qt::gray);
                    drawHexagon(painter, pos, radius);

                    // 移动到下一个六边形
                    cur_q += directions[dir].x();
                    cur_r += directions[dir].y();
                }
            }
        }
    }
}


void RandomMap::drawHexagon(QPainter &painter, const QPointF &center, int radius) {
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3 * i;  // 60度
        double x = center.x() + radius * cos(angle);
        double y = center.y() + radius * sin(angle);
        hexagon << QPointF(x, y);
    }
    painter.drawPolygon(hexagon);
}

RandomMap::~RandomMap()
{

}
