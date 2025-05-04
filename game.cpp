#include "game.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>
#include <QTimer>

Game::Game(QWidget *parent) : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

Game::~Game()
{
    // Qt的父子对象系统会自动处理内存释放
}

// === UI初始化和更新 ===
void Game::initializeUI()
{
    backButton = new Lbutton(this, "返回");
    hintButton = new Lbutton(this, "提示");
    withdrawButton = new Lbutton(this, "撤销");
    pathToggleButton = new Lbutton(this, "显示路径");
    
    backButton->move(0, 0);
    hintButton->move(1400, 200);
    withdrawButton->move(1400, 400);
    pathToggleButton->move(1400, 600);
    
    messageBox = new QMessageBox(this);
    
    gameTimer = new QTimer(this);
    gameTimer->setInterval(1000);
}

void Game::setupConnections()
{
    connect(withdrawButton, &QPushButton::clicked, this, &Game::withdrawLastOperation);
    connect(pathToggleButton, &QPushButton::clicked, this, &Game::togglePathVisibility);
    connect(hintButton, &QPushButton::clicked, this, &Game::showNextHint);
    connect(gameTimer, &QTimer::timeout, this, &Game::updateTimeDisplay);
}

void Game::setMap(MapData mapData)
{
    // 保存关卡ID
    currentLevelId = mapData.id;
    hexagons = mapData.hexagons;
    radius = mapData.radius;
    color1 = mapData.color1;
    color2 = mapData.color2;
    center = mapData.center;
    color3 = mapData.color3;
    path = mapData.path;
    
    // 计算游戏的环数
    rings = 0;
    for (int i = 0; i < hexagons.size(); i++)
    {
        QPoint coord = hexagonIndexToCoord(i);
        rings = qMax(rings, getHexagonRing(coord));
    }
    
    resetGameState();
}

// === 游戏状态管理 ===
void Game::resetGameState()
{
    currentPath.clear();
    operationHistory.clear();
    flippedHexagons.clear();
    currentHintIndex = -1;
    showPath = false;
    penaltySeconds = 0;
    pathToggleButton->setText("显示路径");
    startTimer();
}

bool Game::checkGameComplete()
{
    QMap<int, QColor> ringColors;
    
    for (int i = 0; i < hexagons.size(); i++)
    {
        QPoint coord = hexagonIndexToCoord(i);
        int ring = getHexagonRing(coord);
        
        if (!ringColors.contains(ring))
        {
            ringColors[ring] = hexagons[i].color;
        }
        else if (ringColors[ring] != hexagons[i].color)
        {
            return false;
        }
    }
    return true;
}

void Game::handleGameCompletion()
{
    // 停止计时器
    stopTimer();
    QString message = QString("恭喜完成！用时：%1").arg(timeText);
    messageBox->setText(message);
    messageBox->exec();
    
    // 发送带有完成信息的信号
    emit returnToLevelMode(true, penaltySeconds, currentLevelId);
}

// === 六边形操作 ===
bool Game::canFlipHexagon(int index)
{
    return !flippedHexagons.contains(index) && isConnectedToPath(index);
}

void Game::flipHexagon(int index)
{
    Operation op;
    op.hexagonIndex = index;
    op.oldColor = hexagons[index].color;
    op.hexCoord = hexagonIndexToCoord(index);
    operationHistory.append(op);
    
    hexagons[index].color = (hexagons[index].color == color1) ? color2 : color1;
    flippedHexagons.insert(index);
    currentPath.append(index);
    
    update();
}

int Game::findClosestHexagon(const QPointF& clickPos)
{
    double minDistance = 1E9;
    int closestIndex = -1;
    
    for (int i = 0; i < hexagons.size(); i++)
    {
        double dist = QLineF(clickPos, hexagons[i].center).length();
        if (dist < minDistance)
        {
            minDistance = dist;
            closestIndex = i;
        }
    }
    
    return (minDistance <= radius) ? closestIndex : -1;
}

void Game::withdrawLastOperation()
{
    if (operationHistory.isEmpty()) return;
    
    Operation lastOp = operationHistory.last();
    hexagons[lastOp.hexagonIndex].color = lastOp.oldColor;
    flippedHexagons.remove(lastOp.hexagonIndex);
    operationHistory.removeLast();
    
    if (!currentPath.isEmpty())
    {
        currentPath.removeLast();
    }
    
    update();
    
    // 如果是通过返回按钮退出，发送未完成的信号
    if (sender() == backButton) {
        emit returnToLevelMode(false, penaltySeconds, currentLevelId);
    }
}

// === 路径管理 ===
bool Game::isConnectedToPath(int index)
{
    if (currentPath.isEmpty()) return true;
    
    QPoint coord = hexagonIndexToCoord(index);
    QPoint lastCoord = hexagonIndexToCoord(currentPath.last());
    return areNeighbors(lastCoord, coord);
}

bool Game::areNeighbors(const QPoint& coord1, const QPoint& coord2)
{
    static const QVector<QPair<int, int>> directions = {
        {1,0}, {1,-1}, {0,-1}, {-1,0}, {-1,1}, {0,1}
    };
    
    int dq = coord2.x() - coord1.x();
    int dr = coord2.y() - coord1.y();
    
    for (const auto& dir : directions)
    {
        if (dq == dir.first && dr == dir.second)
        {
            return true;
        }
    }
    return false;
}

void Game::togglePathVisibility()
{
    showPath = !showPath;
    pathToggleButton->setText(showPath ? "隐藏路径" : "显示路径");
    update();
}

// === 事件处理 ===
void Game::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制六边形
    painter.setPen(Qt::gray);
    for (const HexCell &hex : hexagons)
    {
        painter.setBrush(hex.color);
        drawHexagon(painter, hex.center, radius);
    }
    
    // 绘制路径
    if (showPath && !currentPath.isEmpty())
    {
        painter.setPen(QPen(color3, 4));
        for (int i = 0; i < currentPath.size() - 1; ++i)
        {
            QPointF start = hexagons[currentPath[i]].center;
            QPointF end = hexagons[currentPath[i+1]].center;
            painter.drawLine(start, end);
        }
    }
    
    // 绘制计时器
    drawGameTimer(painter);
}

void Game::mousePressEvent(QMouseEvent *event)
{
    int hexIndex = findClosestHexagon(event->pos());
    
    if (hexIndex != -1 && canFlipHexagon(hexIndex))
    {
        flipHexagon(hexIndex);
        
        if (checkGameComplete())
        {
            handleGameCompletion();
        }
    }
}

// === 计时系统 ===
void Game::startTimer()
{
    penaltySeconds = 0;
    updateTimeDisplay();
    gameTimer->start();
}

void Game::stopTimer()
{
    gameTimer->stop();
}

void Game::updateTimeDisplay()
{
    penaltySeconds++;
    int minutes = penaltySeconds / 60;
    int seconds = penaltySeconds % 60;
    timeText = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    update();
}

void Game::drawGameTimer(QPainter &painter)
{
    if (timeText.isEmpty()) return;
    
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(24);
    font.setBold(true);
    painter.setFont(font);
    
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(timeText);
    int textHeight = fm.height();
    int x = (width() - textWidth) / 2;
    int y = textHeight + 20;
    
    QRectF textRect(x - 10, y - textHeight, textWidth + 20, textHeight + 10);
    painter.fillRect(textRect, QColor(0, 0, 0, 128));
    painter.drawText(x, y, timeText);
}

// 将六边形索引转换为轴向坐标
QPoint Game::hexagonIndexToCoord(int index)
{
    if (index < 0 || index >= hexagons.size())
        return QPoint(0, 0);
    
    QPointF hexCenter = hexagons[index].center;
    QPointF relativePos = hexCenter - this->center;
    
    // 使用正确的坐标转换公式
    double q = (2.0/3.0 * relativePos.x()) / radius;
    double r = (-1.0/3.0 * relativePos.x() + sqrt(3.0)/3.0 * relativePos.y()) / radius;
    
    // 四舍五入到最近的整数坐标
    return QPoint(qRound(q), qRound(r));
}

// 将轴向坐标转换为六边形索引
int Game::coordToHexagonIndex(const QPoint& coord)
{
    // 轴向坐标转成笛卡尔坐标
    auto hexToPixel = [this](int q, int r) -> QPointF
    {
        double x = radius * 3.0 / 2 * q;
        double y = radius * sqrt(3) * (r + q / 2.0);
        return center + QPointF(x, y);
    };
    
    QPointF pixel = hexToPixel(coord.x(), coord.y());
    
    // 找到最接近该坐标的六边形
    int closestIndex = -1;
    double minDistance = radius * 2; // 设置一个较大的初始值
    
    for (int i = 0; i < hexagons.size(); i++)
    {
        double distance = QLineF(pixel, hexagons[i].center).length();
        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = i;
        }
    }
    
    // 确保找到的六边形在合理距离内
    if (minDistance > radius / 2)
        return -1;
    
    return closestIndex;
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

void Game::showNextHint()
{
    // 原有的提示逻辑
    if (path.isEmpty()) {
        return;
    }
    
    // 增加罚时（30秒）
    penaltySeconds += 30;
    
    if (currentPath.isEmpty()) {
        QPoint nextCoord = path.first();
        currentHintIndex = 0;
        highlightHexagon(nextCoord);
        return;
    }
    
    bool isMatchingPath = true;
    for (int i = 0; i < currentPath.size(); i++) {
        if (i >= path.size()) {
            isMatchingPath = false;
            break;
        }
        
        QPoint currentCoord = hexagonIndexToCoord(currentPath[i]);
        if (currentCoord != path[i]) {
            isMatchingPath = false;
            break;
        }
    }
    
    if (isMatchingPath && currentPath.size() < path.size()) {
        QPoint nextCoord = path[currentPath.size()];
        highlightHexagon(nextCoord);
    } else {
        withdrawButton->setStyleSheet("background: rgba(255, 255, 255, 0.2);");
        QTimer::singleShot(1000, this, [this]() {
            withdrawButton->setStyleSheet("QPushButton {"
                  "background: rgba(255, 255, 255, 0);"
                  "border: none;"
                  "color:white"
                  "}"

                  "QPushButton:hover {"
                  "background: rgba(255, 255, 255, 0.2);"
                  "}");
        });
    }
}

void Game::highlightHexagon(const QPoint& coord)
{
    // 找到对应的六边形索引
    int targetIndex = -1;
    for (int i = 0; i < hexagons.size(); i++) {
        if (hexagonIndexToCoord(i) == coord) {
            targetIndex = i;
            break;
        }
    }
    
    // 如果找到了目标六边形，高亮显示它
    if (targetIndex >= 0) {
        // 获取当前颜色
        QColor currentColor = hexagons[targetIndex].color;
        
        // 根据当前颜色选择对比色
        QColor highlightColor;
        if (currentColor == color1) {
            // 如果当前是第一种颜色，使用第二种颜色
            highlightColor = color2;
        } else {
            // 如果当前是第一种颜色，使用第一种颜色
            highlightColor = color1;
        }
        
        // 临时保存原始颜色
        QColor originalColor = hexagons[targetIndex].color;
        // 设置高亮颜色
        hexagons[targetIndex].color = highlightColor;
        // 更新显示
        update();
        // 1秒后恢复原始颜色
        QTimer::singleShot(1000, this, [this, targetIndex, originalColor]() {
            hexagons[targetIndex].color = originalColor;
            update();
        });
    }
}

int Game::getHexagonRing(const QPoint& coord)
{
    // 使用轴向坐标计算六边形距离公式：(|q| + |r| + |q+r|) / 2
    int q = coord.x(), r = coord.y();
    return (abs(q) + abs(r) + abs(q + r)) / 2;
}
