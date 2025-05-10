#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include "lbutton.h"
#include "messagebox.h"
#include "mapdata.h"
#include "socketmanager.h"
#include "onlinechat.h"
#include "datamanager.h"
#include <QSpinBox>

// 定义操作结构体，用于记录每次操作
struct Operation {
    int hexagonIndex;  // 操作的六边形索引
    QColor oldColor;   // 操作前的颜色
    QPoint hexCoord;   // 六边形的轴向坐标
};

class Game : public QWidget
{
    Q_OBJECT

public:
    Game(QWidget *parent = nullptr, DataManager *dataManager = nullptr);
    ~Game();
    void setMap(MapData mapData);
    void setSocketManager(SocketManager* manager);  // 设置SocketManager
    void setOnlineMode(bool isServerMode, SocketManager* manager); // 用于联机模式的设置（角色和SocketManager）
    Lbutton *backButton;
    bool getOnlineMode();

signals:
    // 返回关卡模式的信号，携带完成信息
    void returnToLevelMode(bool completed = false, int timeUsed = 0, int steps = 0, int levelId = 0);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onGameStateReceived(const MapData& mapData);  // 处理接收到的游戏状态
    void onRadiusAdjustButtonClicked(); // 处理半径调整按钮点击事件
    void onRadiusSpinBoxChanged(int value); // 处理半径调整输入框变化事件
    void resetCurrentLevel(); // 重置关卡

private:
    // === UI初始化和更新 ===
    void initializeUI();
    void setupConnections();
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);
    void drawGameTimer(QPainter &painter);
    
    // === 游戏状态管理 ===
    void resetGameState(bool fromResetButton = false);
    bool checkGameComplete();
    void handleGameCompletion();
    
    // === 六边形操作 ===
    bool canFlipHexagon(int index);
    void flipHexagon(int index);
    int findClosestHexagon(const QPointF& clickPos);
    void withdrawLastOperation();
    void resetHexagons(const QVector<HexCell>& currentHexagons, int radius1, int radius2);
    
    // === 路径管理 ===
    bool isConnectedToPath(int index);
    bool areNeighbors(const QPoint& coord1, const QPoint& coord2);
    void togglePathVisibility();
    QPoint hexagonIndexToCoord(int index);
    int coordToHexagonIndex(const QPoint& coord);
    int getHexagonRing(const QPoint& coord);
    
    // === 提示系统 ===
    void showNextHint();
    void highlightHexagon(const QPoint& coord);
    
    // === 计时系统 ===
    void updateTimeDisplay();
    void startTimer();
    void stopTimer();

    // === 关卡管理 ===
    void setCurrentLevelId(int id);

    // === 偏移量 ===
    QPointF getOffset();
    
    Lbutton *hintButton;
    Lbutton *withdrawButton;
    Lbutton *pathToggleButton;
    Lbutton *radiusAdjustButton; // 半径调整按钮
    Lbutton *resetButton;        // 重置按钮
    QSpinBox *radiusSpinBox;     // 半径调整输入框
    MessageBox *messageBox;
    QVector<QPoint> path;         // 提示路径
    QVector<int> currentPath;     // 用户当前的操作路径，存储六边形索引
    QVector<HexCell> hexagons;
    QVector<Operation> operationHistory; // 操作历史记录
    QSet<int> flippedHexagons;    // 记录已经翻转过的六边形索引
    bool showPath = false;        // 是否显示路径，默认不显示
    int currentHintIndex = -1;    // 当前提示的索引位置
    QTimer* highlightTimer;       // 高亮效果定时器
    int highlightedHexIndex = -1; // 当前高亮的六边形索引
    bool isShowingHint = false;   // 是否正在显示提示
    bool isAutoRadius = false;    // 是否为自动调整半径模式，默认为手动
    int radius = 50;
    QPointF center = QPointF(850, 440);
    QColor color1, color2, color3;
    QTimer* gameTimer;            // 游戏计时器
    int penaltySeconds = 0;       // 总时间（包含实际时间和罚时）
    QString timeText;             // 时间显示文本
    int rings = 3;               // 游戏的环数
    int stepCount = 0;           // 记录步数
    
    // 添加关卡ID成员变量
    int currentLevelId = 0;

    // 网络同步相关
    SocketManager* socketManager = nullptr;  // SocketManager指针
    bool isServer = false;  // 是否是服务器, 由setOnlineMode设置
    OnlineChat* onlineChat = nullptr; // 联机聊天窗口
    bool isOnlineMode = false; // 是否为联机模式，用于设置聊天窗口是否显示

    DataManager *dataManager;  // 添加 DataManager 指针
};

#endif // GAME_H