#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <cstdlib>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include "game.h"

Game::Game(QWidget *parent, DataManager *dataManager_) : QWidget(parent), dataManager(dataManager_)
{
    initializeUI();
    setupConnections();
}

Game::~Game()
{
}

// === UI初始化和更新 ===
void Game::initializeUI()
{
    backButton = new Lbutton(this, "返回");
    hintButton = new Lbutton(this, "提示");
    withdrawButton = new Lbutton(this, "撤销");
    pathToggleButton = new Lbutton(this, "显示路径");
    radiusAdjustButton = new Lbutton(this, isAutoRadius ? "手动调整半径" : "自动调整半径");
    resetButton = new Lbutton(this, "重置");
    chatButton = new Lbutton(this, "聊天");
    
    // 初始时聊天按钮为禁用状态，只有在联机模式下才启用
    chatButton->setEnabled(false);
    
    radiusSpinBox = new QSpinBox(this);

    radiusSpinBox->setRange(10, 60);
    radiusSpinBox->setValue(radius);
    radiusSpinBox->setEnabled(!isAutoRadius);
    radiusSpinBox->setFixedWidth(140);
    radiusSpinBox->setFixedHeight(50);

    // 创建单独的返回按钮和聊天按钮布局
    QVBoxLayout *backChatLayout = new QVBoxLayout();
    backChatLayout->addWidget(backButton);
    backChatLayout->addStretch(1); // 中间添加弹性空间
    backChatLayout->addWidget(chatButton);
    backChatLayout->setSpacing(10); // 设置这两个按钮之间的间距较小
    backChatLayout->setContentsMargins(10, 10, 10, 10);

    messageBox = new MessageBox(this);

    gameTimer = new QTimer(this);
    gameTimer->setInterval(1000);

    // 创建主要按钮的垂直布局
    QVBoxLayout *buttonColumnLayout = new QVBoxLayout();
    buttonColumnLayout->setSpacing(100); // 设置按钮间的间距
    buttonColumnLayout->addStretch(1); // 按钮组上方的弹性空间
    buttonColumnLayout->addWidget(hintButton);
    buttonColumnLayout->addStretch(1);
    buttonColumnLayout->addWidget(withdrawButton);
    buttonColumnLayout->addStretch(1);
    buttonColumnLayout->addWidget(resetButton);
    buttonColumnLayout->addStretch(1);
    buttonColumnLayout->addWidget(pathToggleButton);
    buttonColumnLayout->addStretch(1);
    buttonColumnLayout->addWidget(radiusAdjustButton);
    buttonColumnLayout->addStretch(1);
    buttonColumnLayout->addWidget(radiusSpinBox);
    buttonColumnLayout->addStretch(1); // 按钮组下方的弹性空间

    // 创建主水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(backChatLayout, 0); // 左侧放置返回和聊天按钮，不拉伸
    mainLayout->addStretch(10); // 中间的弹性空间 (游戏区域)，占据更多比例
    mainLayout->addLayout(buttonColumnLayout, 1); // 右侧的按钮列，占据较小比例

    // 设置主布局的边距
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void Game::setupConnections()
{
    connect(withdrawButton, &QPushButton::clicked, this, &Game::withdrawLastOperation);
    connect(pathToggleButton, &QPushButton::clicked, this, &Game::togglePathVisibility);
    connect(hintButton, &QPushButton::clicked, this, &Game::showNextHint);
    connect(gameTimer, &QTimer::timeout, this, &Game::updateTimeDisplay);
    connect(messageBox->closeButton, &QPushButton::clicked, this, [this]()
    {
        messageBox->accept();
    });
    connect(radiusAdjustButton, &QPushButton::clicked, this, &Game::onRadiusAdjustButtonClicked);
    connect(radiusSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &Game::onRadiusSpinBoxChanged);
    connect(resetButton, &QPushButton::clicked, this, &Game::resetCurrentLevel);
    
    // 添加返回按钮点击事件，隐藏联机聊天窗口
    connect(backButton, &QPushButton::clicked, this, [this]() {
        // 如果在联机模式下，显示退出房间提醒
        if (isOnlineMode && socketManager) {
            if (messageBox) {
                messageBox->setMessage("确定要退出房间吗？");
                int result = messageBox->exec();
                
                if (result == QDialog::Accepted) {
                    // 用户确认退出房间
                    if (socketManager) {
                        socketManager->SendLeaveRoomMessage();
                    }
                    
                    // 隐藏并清空聊天窗口
                    if (onlineChat) {
                        onlineChat->hide();
                        onlineChat->clearChatHistory();
                    }
                    
                    // 发送未完成的信号
                    emit returnToLevelMode(false, penaltySeconds, stepCount, currentLevelId);
                }
                return; // 如果取消，则不执行后续的返回操作
            }
        }
        
        // 非联机模式或无需确认
        if (onlineChat && onlineChat->isVisible()) {
            onlineChat->hide();
        }
        
        // 发送未完成的信号
        emit returnToLevelMode(false, penaltySeconds, stepCount, currentLevelId);
    });
    
    connect(chatButton, &QPushButton::clicked, this, [this]()
    {
        if (onlineChat && isOnlineMode)
        {
            // 如果当前窗口可见，则居中显示
            if (!onlineChat->isVisible())
            {
                // 计算父窗口的中心位置
                QPoint parentCenter = this->mapToGlobal(QPoint(this->width()/2, this->height()/2));
                // 计算聊天窗口应该显示的位置
                QPoint chatPos = QPoint(
                    parentCenter.x() - onlineChat->width()/2,
                    parentCenter.y() - onlineChat->height()/2
                );
                onlineChat->move(chatPos);
            }
            onlineChat->show();
        }
        else
        {
            // 在非联机模式下点击聊天按钮给出提示
            if (messageBox)
            {
                messageBox->setMessage("请先进入联机模式才能使用聊天功能");
                messageBox->exec();
            }
        }
    });
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
    resizeEvent(nullptr);
    update();
}

// === 游戏状态管理 ===
void Game::resetGameState(bool fromResetButton)
{
    currentPath.clear();
    operationHistory.clear();
    flippedHexagons.clear();
    currentHintIndex = -1;

    stepCount = 0;
    isShowingHint = false;
    highlightedHexIndex = -1;

    if (!fromResetButton)
        {
            hintButton->setEnabled(true);
            withdrawButton->setEnabled(true);
            pathToggleButton->setEnabled(true);
            radiusAdjustButton->setEnabled(true);
            radiusSpinBox->setEnabled(!isAutoRadius);
            chatButton->setEnabled(isOnlineMode);
            showPath = true;
            penaltySeconds = 0;
            pathToggleButton->setText("隐藏路径");
            startTimer();
        }


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

    // 仅在非联机模式或服务器模式下发送带有完成信息的信号
    // 客户端模式下不更新排行榜数据
    if (!isOnlineMode || isServer)
        {
            emit returnToLevelMode(true, penaltySeconds, stepCount, currentLevelId);
        }
    else
        {
            // 客户端模式也应发送游戏完成的信号 (true)
            emit returnToLevelMode(false, penaltySeconds, stepCount, currentLevelId);
        }

    if (isOnlineMode)
        {
            hintButton->setEnabled(false);
            withdrawButton->setEnabled(false);
            pathToggleButton->setEnabled(false);
            radiusAdjustButton->setEnabled(false);
            radiusSpinBox->setEnabled(false);
        }

    QString message = QString("恭喜完成！用时：%1").arg(timeText);
    messageBox->setMessage(message);
    messageBox->exec();
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
    stepCount++;

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
    if (operationHistory.isEmpty())
        {
            return;
        }

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
    if (sender() == backButton)
        {
            emit returnToLevelMode(false, penaltySeconds, stepCount, currentLevelId);
        }
}

void Game::resetHexagons(const QVector<HexCell>& currentHexagons, int radius1, int radius2)
{
    //radius1是原始半径，radius2是新半径
    QVector<HexCell> newHexagons;

    double scaleFactor = static_cast<double>(radius2) / static_cast<double>(radius1);
    QPointF pivotCenter = center; // 使用 this->center 作为缩放的中心点

    newHexagons.reserve(currentHexagons.size()); // 预分配内存以提高效率

    for (const HexCell& oldCell : currentHexagons)
        {
            HexCell newCell;
            newCell.color = oldCell.color; // 保持颜色不变

            // 计算六边形中心相对于 pivotCenter 的位置
            QPointF relativePos = oldCell.center - pivotCenter;

            // 根据缩放因子调整相对位置
            QPointF newRelativePos = relativePos * scaleFactor;

            // 计算新的绝对中心位置
            newCell.center = pivotCenter + newRelativePos;

            newHexagons.append(newCell);
        }

    hexagons = newHexagons;
}

// === 路径管理 ===
bool Game::isConnectedToPath(int index)
{
    if (currentPath.isEmpty())
        {
            return true;
        }

    QPoint coord = hexagonIndexToCoord(index);
    QPoint lastCoord = hexagonIndexToCoord(currentPath.last());
    return areNeighbors(lastCoord, coord);
}

bool Game::areNeighbors(const QPoint& coord1, const QPoint& coord2)
{
    static const QVector<QPair<int, int>> directions =
    {
        {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}
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
            QPointF offset = getOffset();
            painter.setPen(QPen(color3, 4));
            for (int i = 0; i < currentPath.size() - 1; ++i)
                {
                    QPointF start = hexagons[currentPath[i]].center + offset;
                    QPointF end = hexagons[currentPath[i + 1]].center + offset;
                    painter.drawLine(start, end);
                }
        }

    // 绘制计时器
    drawGameTimer(painter);
}

void Game::mousePressEvent(QMouseEvent *event)
{
    static int clickCount = -1;
    clickCount++;

    // 在联机模式下，如果游戏已完成，则不处理点击事件
    if (clickCount && isOnlineMode && checkGameComplete())
        {
            return;
        }

    // 如果正在显示提示，忽略点击事件
    if (isShowingHint)
        {
            return;
        }

    QPointF offset = getOffset();

    int hexIndex = findClosestHexagon(event->pos() - offset);

    if (hexIndex != -1 && canFlipHexagon(hexIndex))
        {
            flipHexagon(hexIndex);

            if (checkGameComplete())
                {
                    handleGameCompletion();
                }
        }
}

void Game::resizeEvent(QResizeEvent *event)
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
    int textHeight = fm.height();

    // 修改位置到左侧正中间
    int x = 10; // 左侧位置
    int y = height() / 2; // 垂直居中

    painter.drawText(x, y + textHeight / 2 - fm.descent(), timeText);
}

// 将六边形索引转换为轴向坐标
QPoint Game::hexagonIndexToCoord(int index)
{
    if (index < 0 || index >= hexagons.size())
        return QPoint(0, 0);

    QPointF hexCenter = hexagons[index].center;
    QPointF relativePos = hexCenter - this->center;

    // 使用正确的坐标转换公式
    double q = (2.0 / 3.0 * relativePos.x()) / radius;
    double r = (-1.0 / 3.0 * relativePos.x() + sqrt(3.0) / 3.0 * relativePos.y()) / radius;

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
        {
            return -1;
        }

    return closestIndex;
}

void Game::drawHexagon(QPainter &painter, const QPointF &center, int radius)
{
    QPointF offset = getOffset();
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

void Game::showNextHint()
{
    if (path.isEmpty()) // 没有解法路径
        {
            if(messageBox)
                {
                    messageBox->setMessage("当前关卡没有可用提示");
                    messageBox->exec();
                }
            return;
        }

    // 增加罚时
    penaltySeconds += 30;

    if (currentPath.isEmpty()) // 玩家未开始
        {
            currentHintIndex = 0;
            highlightHexagon(path.first());
            return;
        }

    // --- 检查是否重置 ---
    int diffCount = 0;
    int commonLength = 0;
    for (int i = 0; i < qMin(currentPath.size(), path.size()); ++i)
        {
            if (hexagonIndexToCoord(currentPath[i]) != path[i])
                {
                    diffCount = currentPath.size() - i;
                    break;
                }
            commonLength++;
        }

    // 如果玩家路径比标准路径长，且前面都匹配，则多出部分算差异
    if (!diffCount && currentPath.size() > path.size())
        {
            diffCount = currentPath.size() - path.size();
        }

    // 只有当差异超过一半才重置
    if ( diffCount > commonLength)
        {
            resetButton->setStyleSheet("background: rgba(255, 255, 255, 0.2);");
            QTimer::singleShot(250, this, [this]()
            {
                resetButton->setStyleSheet("QPushButton {"
                                           "background: rgba(255, 255, 255, 0);"
                                           "border: none;"
                                           "color:white"
                                           "}"

                                           "QPushButton:hover {"
                                           "background: rgba(255, 255, 255, 0.2);"
                                           "}");
            });
        }
    else
        {
            bool isMatchingPath = true;
            for (int i = 0; i < currentPath.size(); i++)
                {
                    if (i >= path.size())
                        {
                            isMatchingPath = false;
                            break;
                        }

                    QPoint currentCoord = hexagonIndexToCoord(currentPath[i]);
                    if (currentCoord != path[i])
                        {
                            isMatchingPath = false;
                            break;
                        }
                }
            if(isMatchingPath && currentPath.size() == path.size())
                {
                    qDebug() << "path错误！！！！";
                }

            if (isMatchingPath && currentPath.size() < path.size())
                {
                    QPoint nextCoord = path[currentPath.size()];
                    highlightHexagon(nextCoord);
                }
            else
                {
                    withdrawButton->setStyleSheet("background: rgba(255, 255, 255, 0.2);");
                    QTimer::singleShot(250, this, [this]()
                    {
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

}

void Game::highlightHexagon(const QPoint& coord)
{
    if(isShowingHint)
        {
            return;
        }

    // 找到对应的六边形索引
    int targetIndex = -1;
    for (int i = 0; i < hexagons.size(); i++)
        {
            if (hexagonIndexToCoord(i) == coord)
                {
                    targetIndex = i;
                    break;
                }
        }

    // 如果找到了目标六边形，高亮显示它
    if (targetIndex >= 0)
        {
            // 获取当前颜色
            QColor currentColor = hexagons[targetIndex].color;

            // 根据当前颜色选择对比色
            QColor highlightColor;
            if (currentColor == color1)
                {
                    // 如果当前是第一种颜色，使用第二种颜色
                    highlightColor = color2;
                }
            else
                {
                    // 如果当前是第一种颜色，使用第一种颜色
                    highlightColor = color1;
                }

            // 设置正在显示提示标志
            isShowingHint = true;
            highlightedHexIndex = targetIndex;

            // 临时保存原始颜色
            QColor originalColor = hexagons[targetIndex].color;
            // 设置高亮颜色
            hexagons[targetIndex].color = highlightColor;
            // 更新显示
            update();
            // 250毫秒后恢复原始颜色
            QTimer::singleShot(250, this, [this, targetIndex, originalColor]()
            {

                hexagons[targetIndex].color = originalColor;
                isShowingHint = false;
                highlightedHexIndex = -1;
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

void Game::setCurrentLevelId(int id)
{
    currentLevelId = id;
}

void Game::setSocketManager(SocketManager* manager)
{
    socketManager = manager;
    connect(socketManager, &SocketManager::gameStateReceived, this, &Game::onGameStateReceived);
}

void Game::onGameStateReceived(const MapData& mapData)
{
    // 如果不是服务器，则更新游戏状态
    if (!isServer)
        {
            setMap(mapData);
            update();
        }
}

void Game::onRadiusAdjustButtonClicked()
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

void Game::onRadiusSpinBoxChanged(int value)
{
    if (!isAutoRadius)
        {
            int oldRadius = radius;
            radius = value;
            resetHexagons(hexagons, oldRadius, radius);
            update();
        }
}

void Game::setOnlineMode(bool isServer_, SocketManager* manager)
{
    isServer = isServer_;
    isOnlineMode = (manager != nullptr);  // 根据 manager 是否为空来设置联机模式

    if (manager)
        {
            setSocketManager(manager);
            
            // 创建或重用聊天窗口
            if (onlineChat == nullptr)
                {
                    onlineChat = new OnlineChat(manager, dataManager, this);
                }
            
            // 设置窗口标题
            onlineChat->setWindowTitle("联机聊天");
            
            // 清空历史聊天记录（重新进入房间）
            onlineChat->clearChatHistory();
            
            // 加载用户头像 - 无论是否已有用户头像，都重新获取一次
            if (socketManager) {
                QString localUserId = socketManager->getLocalUserId();
                
                // 重置头像设置，强制重新获取
                onlineChat->setUserAvatar(localUserId);
                
                // 设置短延迟后发送头像，确保头像数据能够被接收方处理
                QTimer::singleShot(300, onlineChat, &OnlineChat::sendCurrentUserAvatar);
                
                // 再次延迟发送一次，以确保稳定传输
                QTimer::singleShot(1000, onlineChat, &OnlineChat::sendCurrentUserAvatar);
            }
            
            // 启用聊天按钮
            if (chatButton)
                {
                    chatButton->setEnabled(true);
                }
        }
    else
        {
            // 隐藏聊天窗口
            if (onlineChat)
                {
                    onlineChat->hide();
                }
            socketManager = nullptr;
            
            // 禁用聊天按钮
            if (chatButton)
                {
                    chatButton->setEnabled(false);
                }
        }
}

bool Game::getOnlineMode()
{
    return isOnlineMode;
}

// === 偏移量 ===
QPointF Game::getOffset()
{
    // 获取当前窗口的中心点
    QPointF currentWidgetCenter = QPointF(this->width() / 2.0, this->height() / 2.0);
    // 获取游戏布局的原始中心点 (从 MapData 加载到 this->center 成员变量)
    QPointF originalLayoutCenter = this->center;

    // 计算偏移量：当前窗口中心 - 原始布局中心
    QPointF offset = currentWidgetCenter - originalLayoutCenter;
    return offset;
}

void Game::resetCurrentLevel()
{
    // 核心重置逻辑: 获取初始地图数据以恢复六边形颜色等
    if (dataManager && currentLevelId != 0)   // 确保 currentLevelId 有效
        {
            MapData initialMapData = dataManager->getMap(currentLevelId);
            // 恢复六边形颜色到初始状态
            for(int i = 0; i < hexagons.size() && i < initialMapData.hexagons.size(); ++i)
                {
                    hexagons[i].color = initialMapData.hexagons[i].color;
                }
        }
    else
        {
            qWarning() << "resetCurrentLevel: Cannot reset colors from DataManager due to invalid currentLevelId or no DataManager.";
        }

    // 调用 resetGameState() 清理玩家进度、计时器（部分）、按钮状态等
    resetGameState(true);

    // 触发界面刷新
    update();
}



