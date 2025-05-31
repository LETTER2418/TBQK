#include "randommap.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>
#include <QGridLayout>
#include <QResizeEvent>

RandomMap::RandomMap(QWidget *parent) : QWidget(parent)
{
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);
    saveButton = new Lbutton(this, "保存");
    saveButton->enableClickEffect(true);
    id = 0;

    // 创建半径调整按钮和SpinBox
    radiusAdjustButton = new Lbutton(this, "自动调整半径");
    radiusAdjustButton->enableClickEffect(true);

    radiusSpinBox = new QSpinBox(this);
    radiusSpinBox->setRange(10, 60);
    radiusSpinBox->setValue(radius);
    radiusSpinBox->setEnabled(!isAutoRadius);
    radiusSpinBox->setFixedWidth(140);
    radiusSpinBox->setFixedHeight(50);

    QGridLayout *mainLayout = new QGridLayout(this);

    // saveButton 放置在第1行第2列
    mainLayout->setSpacing(100);
    mainLayout->addWidget(saveButton, 1, 2);
    mainLayout->addWidget(radiusAdjustButton, 2, 2);
    mainLayout->addWidget(radiusSpinBox, 3, 2);

    // 设置列伸展
    mainLayout->setColumnStretch(0, 0); // 第0列 不伸展
    mainLayout->setColumnStretch(1, 1); // 第1列 (中间列) 伸展，占据多余空间
    mainLayout->setColumnStretch(2, 0); // 第2列 不伸展

    // 设置行伸展
    // mainLayout->setRowStretch(0, 1);
    mainLayout->setRowStretch(1, 1);
    mainLayout->setRowStretch(2, 1);
    mainLayout->setRowStretch(3, 1);

    setLayout(mainLayout);

    // 连接信号和槽
    connect(radiusAdjustButton, &QPushButton::clicked, this, &RandomMap::onRadiusAdjustButtonClicked);
    connect(radiusSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &RandomMap::onRadiusSpinBoxChanged);
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
            {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}};

    auto hexToPixel = [=](int q, int r) -> QPointF
    {
        double x = radius * 3.0 / 2 * q;
        double y = radius * std::sqrt(3) * (r + q / 2.0);
        return center + QPointF(x, y);
    };

    QSet<QPoint> visited;
    // 随机生成起点坐标
    int startQ = rand() % (2 * rings + 1) - rings;
    int startR = rand() % (2 * rings + 1) - rings;
    // 确保起点在合法范围内
    while (abs(startQ + startR) > rings)
    {
        startQ = rand() % (2 * rings + 1) - rings;
        startR = rand() % (2 * rings + 1) - rings;
    }
    QPoint current(startQ, startR);
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
    // int f = rand() % 2;
    QVector<bool> ringFlipStatus(rings + 1);
    for (int ring = 0; ring <= rings; ++ring)
    {
        // ringFlipStatus[ring] = ++f % 2 == 0; // 决定是否翻转
        ringFlipStatus[ring] = rand() % 2 == 0; // 决定是否翻转
        if (ring > 0)
        {
            ringFlipStatus[ring] = rand() % 3 ? 1 - ringFlipStatus[ring - 1] : ringFlipStatus[ring - 1];
        }
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
    // 获取当前窗口的中心点
    QPointF currentWidgetCenter = QPointF(this->width() / 2.0, this->height() / 2.0);
    // 获取游戏布局的原始中心点 (从 MapData 加载到 this->center 成员变量)
    QPointF originalLayoutCenter = this->center;

    // 计算偏移量：当前窗口中心 - 原始布局中心
    QPointF offset = currentWidgetCenter - originalLayoutCenter;

    // 将偏移量应用到传入的六边形逻辑中心点上，得到最终的绘制中心点
    QPointF finalDrawCenter = center + offset;

    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i)
    {
        double angle = M_PI / 3 * i;
        // 使用 finalDrawCenter 进行绘制
        double x = finalDrawCenter.x() + radius * cos(angle);
        double y = finalDrawCenter.y() + radius * sin(angle);
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

void RandomMap::onRadiusAdjustButtonClicked()
{
    isAutoRadius = !isAutoRadius;
    radiusAdjustButton->setText(isAutoRadius ? "手动调整半径" : "自动调整半径");
    radiusSpinBox->setEnabled(!isAutoRadius);

    if (isAutoRadius)
    {
        resizeEvent(nullptr);
    }
    else
    {
        radiusSpinBox->setValue(radius);
    }
    update();
}

void RandomMap::onRadiusSpinBoxChanged(int value)
{
    if (!isAutoRadius)
    {
        int oldRadius = radius;
        radius = value;
        resetHexagons(hexagons, oldRadius, radius);
        update();
    }
}

void RandomMap::resetHexagons(const QVector<HexCell> &currentHexagons, int radius1, int radius2)
{
    // radius1是原始半径，radius2是新半径
    QVector<HexCell> newHexagons;
    double scaleFactor = static_cast<double>(radius2) / static_cast<double>(radius1);
    QPointF pivotCenter = center; // 使用中心点作为缩放的中心点

    newHexagons.reserve(currentHexagons.size());

    for (const HexCell &oldCell : currentHexagons)
    {
        HexCell newCell;
        newCell.color = oldCell.color; // 保持颜色不变

        // 计算六边形中心相对于pivotCenter的位置
        QPointF relativePos = oldCell.center - pivotCenter;

        // 根据缩放因子调整相对位置
        QPointF newRelativePos = relativePos * scaleFactor;

        // 计算新的绝对中心位置
        newCell.center = pivotCenter + newRelativePos;

        newHexagons.append(newCell);
    }

    hexagons = newHexagons;
}

void RandomMap::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    int oldRadius = radius;

    if (isAutoRadius)
    {
        int newRadius = 50;
        if (rings == 3)
        {
            float ratio = 0.61;
            int radius1 = ratio / 2 / (2 * rings + 1) * width();
            int radius2 = ratio / sqrt(3) / (2 * rings + 1) * height();
            newRadius = fmin(radius1, radius2);
        }
        else if (rings == 4)
        {
            float ratio = 0.61;
            int radius1 = ratio / 2 / (2 * rings + 1) * width();
            int radius2 = ratio / sqrt(3) / (2 * rings + 1) * height();
            newRadius = fmin(radius1, radius2);
        }
        radius = newRadius;
        radiusSpinBox->blockSignals(true);
        radiusSpinBox->setValue(radius);
        radiusSpinBox->blockSignals(false);
    }
    resetHexagons(hexagons, oldRadius, radius);
    update();
}
