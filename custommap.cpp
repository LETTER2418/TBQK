#include "custommap.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>
#include <QQueue>
#include <QSet>
#include <QPair>
#include <QMap>
#include <algorithm>
#include <QGridLayout>
#include <QResizeEvent>

CustomMap::CustomMap(QWidget *parent) : QWidget(parent)
{
    id = 0;
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);
    saveButton = new Lbutton(this, "保存");
    saveButton->enableClickEffect(true);

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
    mainLayout->setSpacing(100);

    // saveButton 放置在第1行第2列
    mainLayout->addWidget(saveButton, 1, 2);
    mainLayout->addWidget(radiusAdjustButton, 2, 2);
    mainLayout->addWidget(radiusSpinBox, 3, 2);

    // 设置列伸展
    mainLayout->setColumnStretch(0, 0); // 第0列 不伸展
    mainLayout->setColumnStretch(1, 1); // 第1列 (中间列) 伸展，占据多余空间
    mainLayout->setColumnStretch(2, 0); // 第2列 不伸展

    // 设置行伸展
    mainLayout->setRowStretch(1, 1);
    mainLayout->setRowStretch(2, 1);
    mainLayout->setRowStretch(3, 1);

    setLayout(mainLayout);

    // 连接信号和槽
    connect(radiusAdjustButton, &QPushButton::clicked, this, &CustomMap::onRadiusAdjustButtonClicked);
    connect(radiusSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CustomMap::onRadiusSpinBoxChanged);
}

CustomMap::~CustomMap()
{
}

// 生成六边形网格地图
void CustomMap::generateHexagons(int r, QColor c1, QColor c2, QColor c3)
{
    path.clear();
    hexagons.clear();
    color1 = c1;
    color2 = c2;
    color3 = c3;
    rings = r;

    // 轴向坐标转成笛卡尔坐标
    auto hexToPixel = [=](int q, int r) -> QPointF
    {
        double x = radius * 3.0 / 2 * q;
        double y = radius * sqrt(3) * (r + q / 2.0);
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

    QPointF offset = getOffset();
    for (const HexCell &hex : hexagons)
    {
        painter.setBrush(hex.color);
        drawHexagon(painter, hex.center + offset, radius);
    }
}

void CustomMap::mousePressEvent(QMouseEvent *event)
{
    QPointF offset = getOffset();
    QPointF click = event->pos() - offset;
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

void CustomMap::drawHexagon(QPainter &painter, const QPointF &center, int radius)
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

MapData CustomMap::getMapData()
{
    MapData data;
    data.hexagons = this->hexagons; // 复制六边形单元格数据
    data.radius = this->radius;     // 复制六边形半径
    data.center = this->center;     // 复制地图中心坐标
    data.color1 = this->color1;     // 复制第一种颜色
    data.color2 = this->color2;     // 复制第二种颜色
    data.color3 = this->color3;     // 复制第三种颜色
    data.id = this->id;             // 复制关卡索引
    data.path = this->path;         // 复制路径数据
    data.generation = "custom";     // 设置为自定义生成
    return data;
}

void CustomMap::setId(int id_)
{
    id = id_;
}

// 求解六边形拼图
int CustomMap::solvePuzzle()
{
    path.clear();

    // 六边形距离计算函数
    auto distanceFromCenter = [](const QPair<int, int> &h)
    {
        // 使用轴向坐标计算六边形距离公式：(|q| + |r| + |q+r|) / 2
        int q = h.first, r = h.second;
        return (abs(q) + abs(r) + abs(q + r)) / 2;
    };

    // 六边形方向定义
    const QVector<QPair<int, int>> directions =
        {
            {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}};

    // 构建坐标到索引的映射
    QMap<QPair<int, int>, int> coordToIndex;
    QVector<QVector<int>> adj;

    // BFS 生成所有在盘面内的 QPair<int, int> 坐标，并赋索引
    QQueue<QPair<int, int>> q;
    QSet<QPair<int, int>> visited;

    QPair<int, int> center = {0, 0};
    q.push_back(center);
    visited.insert(center);

    int index = 0;
    coordToIndex[center] = index++;
    QVector<QPair<int, int>> all_coords;
    all_coords.push_back(center);

    while (!q.empty())
    {
        QPair<int, int> cur = q.front();
        q.pop_front();
        int cq = cur.first, cr = cur.second;
        for (const auto &[dq, dr] : directions)
        {
            QPair<int, int> neighbor = {cq + dq, cr + dr};
            if (!visited.contains(neighbor) && distanceFromCenter(neighbor) <= rings)
            {
                visited.insert(neighbor);
                q.push_back(neighbor);
                coordToIndex[neighbor] = index++;
                all_coords.push_back(neighbor);
            }
        }
    }

    // 构造邻接表
    adj.resize(index); // 初始化邻接表大小
    for (const QPair<int, int> &coord : all_coords)
    {
        int u = coordToIndex[coord];
        int q = coord.first, r = coord.second;
        for (const auto &[dq, dr] : directions)
        {
            QPair<int, int> neighbor = {q + dq, r + dr};
            if (coordToIndex.contains(neighbor))
            {
                int v = coordToIndex[neighbor];
                adj[u].push_back(v);
            }
        }
    }

    // 枚举每个同心环的颜色
    QVector<QColor> ringColors(rings + 1);
    QVector<int> flipState(index, 0); // 每个六边形的翻转状态
    QMap<int, QColor> indexToColor;   // 存储每个索引对应的六边形颜色

    // 建立索引到颜色的映射
    for (int i = 0; i < hexagons.size(); ++i)
    {
        for (const auto &coord : all_coords)
        {
            // 找到六边形的坐标
            QPointF hexPoint = hexagons[i].center;

            // 轴向坐标转成笛卡尔坐标
            double x = radius * 3.0 / 2 * coord.first;
            double y = radius * sqrt(3) * (coord.second + coord.first / 2.0);
            QPointF pixel = this->center + QPointF(x, y);

            // 如果坐标接近，建立映射
            if (QLineF(hexPoint, pixel).length() < radius / 2)
            {
                int idx = coordToIndex[coord];
                indexToColor[idx] = hexagons[i].color;
                break;
            }
        }
    }

    // 存储所有可行解
    struct Solution
    {
        int count;                 // 翻转的六边形数量
        int pattern;               // 环颜色模式
        QVector<int> flippedCells; // 翻转的六边形索引
    };

    QVector<Solution> solutions;

    // 枚举所有可能的同心环颜色组合
    for (int colorPattern = 0; colorPattern < (1 << (rings + 1)); ++colorPattern)
    {
        // 重置翻转状态
        for (int i = 0; i < index; ++i)
        {
            flipState[i] = 0;
        }

        // 设置每个环的颜色
        for (int r = 0; r <= rings; ++r)
        {
            ringColors[r] = (colorPattern & (1 << r)) ? color2 : color1;
        }

        // 判断每个六边形的flipState
        for (const auto &coord : all_coords)
        {
            int idx = coordToIndex[coord];
            int ring = distanceFromCenter(coord);
            QColor ringColor = ringColors[ring];

            if (indexToColor.contains(idx) && indexToColor[idx] != ringColor)
            {
                flipState[idx] = 1;
            }
        }

        // 检查flipState为1的六边形是否联通
        QVector<bool> visited2(index, false);
        int count = 0;

        // 找到第一个flipState为1的六边形
        int start = -1;
        for (int i = 0; i < index; ++i)
        {
            if (flipState[i] == 1)
            {
                start = i;
                break;
            }
        }

        if (start != -1)
        {
            // BFS检查连通性
            QQueue<int> bfsQueue;
            bfsQueue.enqueue(start);
            visited2[start] = true;
            count = 1;

            while (!bfsQueue.empty())
            {
                int u = bfsQueue.dequeue();

                for (int v : adj[u])
                {
                    if (!visited2[v] && flipState[v] == 1)
                    {
                        visited2[v] = true;
                        bfsQueue.enqueue(v);
                        count++;
                    }
                }
            }

            // 统计所有flipState为1的六边形数量
            int totalFlipped = 0;
            QVector<int> flippedCells;
            for (int i = 0; i < index; ++i)
            {
                if (flipState[i] == 1)
                {
                    totalFlipped++;
                    flippedCells.push_back(i);
                }
            }

            // 如果所有翻转的六边形都能通过BFS访问到，说明它们是联通的
            if (count == totalFlipped && count > 0)
            {
                // 将有效解加入solutions
                solutions.push_back({count, colorPattern, flippedCells});
            }
        }
    }

    // 枚举所有解决方案
    if (!solutions.empty())
    {
        bool foundValidSolution = false;
        int minFlips = INT_MAX;
        QVector<QPoint> bestSolutionPath;
        Solution bestSolution;

        // 遍历所有解决方案
        for (int solIdx = 0; solIdx < solutions.size(); ++solIdx)
        {
            const Solution &sol = solutions[solIdx];

            // 构建需要翻转的六边形构成的图的邻接表
            QVector<QVector<int>> flipGraph;
            flipGraph.resize(sol.flippedCells.size());

            // 建立索引映射，从原始索引到新图中的索引
            QMap<int, int> originalToFlipIndex;
            for (int i = 0; i < sol.flippedCells.size(); ++i)
            {
                originalToFlipIndex[sol.flippedCells[i]] = i;
            }

            // 构建新的邻接表
            for (int i = 0; i < sol.flippedCells.size(); ++i)
            {
                int originalIdx = sol.flippedCells[i];

                // 检查原始图中的邻居是否也在需要翻转的集合中
                for (int neighbor : adj[originalIdx])
                {
                    if (originalToFlipIndex.contains(neighbor))
                    {
                        int flipNeighborIdx = originalToFlipIndex[neighbor];
                        flipGraph[i].push_back(flipNeighborIdx);
                    }
                }
            }

            // 使用模拟退火判断是否能一笔联通，并返回最佳路径
            QVector<int> bestPath;
            bool canConnectInOneStroke = simulatedAnnealing(flipGraph, bestPath);

            // 只处理能一笔联通的解决方案
            if (canConnectInOneStroke)
            {
                foundValidSolution = true;

                // 如果找到翻转数更少的解决方案，更新最佳解
                if (sol.count < minFlips)
                {
                    minFlips = sol.count;
                    bestSolution = sol;

                    // 将最佳路径转换为六边形坐标
                    QVector<QPoint> tempPath;
                    for (int idx : bestPath)
                    {
                        // 获取在原始图中的索引
                        int originalIdx = sol.flippedCells[idx];

                        // 找到对应的坐标
                        for (const auto &[coord, index] : coordToIndex.toStdMap())
                        {
                            if (index == originalIdx)
                            {
                                // 将六边形的轴向坐标添加到路径中
                                tempPath.append(QPoint(coord.first, coord.second));
                                break;
                            }
                        }
                    }
                    bestSolutionPath = tempPath;
                }
            }
        }

        if (!foundValidSolution)
        {
            return 2;
        }
        else
        {
            // 保存最佳路径到成员变量
            path = bestSolutionPath;
            // for (const QPoint &p : path)
            //     {
            //          std::cout << "(" << p.x() << "," << p.y() << ") ";
            //     }
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

// 计算路径的得分（访问的边数）
double CustomMap::calculatePathScore(const QVector<int> &path, const QVector<QVector<int>> &graph)
{
    int score = 0;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        int u = path[i];
        int v = path[i + 1];
        // 检查是否存在边
        for (int next : graph[u])
        {
            if (next == v)
            {
                score++;
                break;
            }
        }
    }
    return score;
}

// 模拟退火算法判断是否能一笔联通，并返回最佳路径
bool CustomMap::simulatedAnnealing(const QVector<QVector<int>> &graph, QVector<int> &bestPath)
{
    if (graph.empty())
    {
        return false;
    }

    const int n = graph.size();
    const double initialTemp = 100.0;
    const double coolingRate = 0.995;
    const int maxIterations = 10000000;

    // 初始化随机路径
    QVector<int> currentPath;
    for (int i = 0; i < n; ++i)
    {
        currentPath.push_back(i);
    }
    std::random_shuffle(currentPath.begin(), currentPath.end());

    double currentScore = calculatePathScore(currentPath, graph);
    double bestScore = currentScore;
    bestPath = currentPath;

    double temperature = initialTemp;

    // 模拟退火主循环
    // 加入限制最大时间，取消iter限制
    for (int iter = 0; iter < maxIterations && bestScore != n - 1 && temperature > 0.001; ++iter)
    {
        // 生成新解：随机交换两个位置
        QVector<int> newPath = currentPath;
        int i = rand() % n;
        int j = rand() % n;
        std::swap(newPath[i], newPath[j]);

        // 计算新解的得分
        double newScore = calculatePathScore(newPath, graph);

        // 计算接受概率
        double delta = newScore - currentScore;
        double acceptanceProbability = exp(delta / temperature);

        // 接受新解或保持当前解
        if (delta > 0 || (double)rand() / RAND_MAX < acceptanceProbability)
        {
            currentPath = newPath;
            currentScore = newScore;

            if (currentScore > bestScore)
            {
                bestScore = currentScore;
                bestPath = currentPath;
            }
        }

        // 降温
        temperature *= coolingRate;
    }

    // 判断是否找到一笔联通的路径
    // 边数应该等于节点数-1，且所有节点都在路径中
    return bestScore == n - 1;
}

void CustomMap::onRadiusAdjustButtonClicked()
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

void CustomMap::onRadiusSpinBoxChanged(int value)
{
    if (!isAutoRadius)
    {
        int oldRadius = radius;
        radius = value;
        resetHexagons(hexagons, oldRadius, radius);
        update();
    }
}

void CustomMap::resetHexagons(const QVector<HexCell> &currentHexagons, int radius1, int radius2)
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

void CustomMap::resizeEvent(QResizeEvent *event)
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

QPointF CustomMap::getOffset()
{
    // 获取当前窗口的中心点
    QPointF currentWidgetCenter = QPointF(this->width() / 2.0, this->height() / 2.0);
    // 获取游戏布局的原始中心点
    QPointF originalLayoutCenter = this->center;

    // 计算偏移量：当前窗口中心 - 原始布局中心
    return currentWidgetCenter - originalLayoutCenter;
}
