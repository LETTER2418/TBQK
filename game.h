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
#include <QMap>
#include <QSet>
#include <QVector>
#include <QColor>
#include <QPoint>
#include <QPointF>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>

// 定义操作结构体，用于记录每次操作
struct Operation
{
    int hexagonIndex; // 操作的六边形索引
    QColor oldColor;  // 操作前的颜色
};

// 六边形状态枚举
enum class HexState
{
    Normal,        // 正常状态
    Dissolving,    // 分解中
    Dissolved,     // 已分解
    Reconstructing // 重构中
};

// 粒子结构体
struct HexParticle
{
    QPointF pos;       // 当前位置
    QPointF velocity;  // 移动速度
    QPointF startPos;  // 起始位置
    QPointF targetPos; // 目标位置
    QColor color;      // 颜色
    float size;        // 大小
    float alpha;       // 透明度
    float life;        // 生命周期
    float maxLife;     // 最大生命周期
};

class Game : public QWidget
{
    Q_OBJECT

public:
    Game(QWidget *parent = nullptr, DataManager *dataManager = nullptr);
    ~Game();
    void setMap(MapData mapData);
    void setSocketManager(SocketManager *manager);                 // 设置SocketManager
    void setOnlineMode(bool isServerMode, SocketManager *manager); // 用于联机模式的设置（角色和SocketManager）
    Lbutton *backButton;
    bool getOnlineMode();
    OnlineChat *getOnlineChat() const
    {
        return onlineChat;
    }

    // 触发所有六边形产生粒子效果
    void triggerAllHexEffects();

signals:
    // 返回关卡模式的信号，携带完成信息
    void returnToLevelMode(bool completed = false, int timeUsed = 0, int steps = 0, int levelId = 0);
    // 通知对方完成关卡的信号
    void levelCompleted(int timeUsed);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onRadiusAdjustButtonClicked();          // 处理半径调整按钮点击事件
    void onRadiusSpinBoxChanged(int value);      // 处理半径调整输入框变化事件
    void resetCurrentLevel();                    // 重置关卡
    void onOpponentLevelCompleted(int timeUsed); // 处理对方完成关卡的消息

    // 六边形粒子特效相关槽函数
    void updateParticles();
    void onDissolveFinished(int hexIndex);
    void startReconstruction(int hexIndex);
    void onReconstructFinished(int hexIndex);
    void triggerHexDissolveEffect(int hexIndex);

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
    int findClosestHexagon(const QPointF &clickPos);
    void withdrawLastOperation();
    void resetHexagons(const QVector<HexCell> &currentHexagons, int radius1, int radius2);

    // === 路径管理 ===
    bool isConnectedToPath(int index);
    bool areNeighbors(const QPoint &coord1, const QPoint &coord2);
    void togglePathVisibility();
    QPoint hexagonIndexToCoord(int index);
    int coordToHexagonIndex(const QPoint &coord);
    int getHexagonRing(const QPoint &coord);

    // === 提示系统 ===
    void showNextHint();
    void highlightHexagon(const QPoint &coord);

    // === 计时系统 ===
    void updateTimeDisplay();
    void startTimer();
    void stopTimer();

    // === 关卡管理 ===
    void setCurrentLevelId(int id);

    // === 偏移量 ===
    QPointF getOffset();

    // === 六边形粒子特效相关方法 ===
    void initParticleSystem();
    void createDissolveParticles(int hexIndex);
    void updateDissolveEffect(int hexIndex);
    void updateReconstructEffect(int hexIndex);
    void generateHexOutline(int hexIndex, QVector<QPointF> &outlinePoints);
    QPointF getRandomPointAround(const QPointF &center, float radius);
    void drawParticles(QPainter &painter);

    Lbutton *hintButton;
    Lbutton *withdrawButton;
    Lbutton *pathToggleButton;
    Lbutton *radiusAdjustButton; // 半径调整按钮
    Lbutton *resetButton;        // 重置按钮
    Lbutton *chatButton;         // 聊天按钮
    QSpinBox *radiusSpinBox;     // 半径调整输入框
    MessageBox *messageBox;
    QVector<QPoint> path;     // 提示路径
    QVector<int> currentPath; // 用户当前的操作路径，存储六边形索引
    QVector<HexCell> hexagons;
    QVector<Operation> operationHistory; // 操作历史记录
    QSet<int> flippedHexagons;           // 记录已经翻转过的六边形索引
    bool showPath = false;               // 是否显示路径，默认不显示
    int currentHintIndex = -1;           // 当前提示的索引位置
    int highlightedHexIndex = -1;        // 当前高亮的六边形索引
    bool isShowingHint = false;          // 是否正在显示提示
    bool isAutoRadius = false;           // 是否为自动调整半径模式，默认为手动
    int radius = 50;
    QPointF center = QPointF(850, 440);
    QColor color1, color2, color3;
    QTimer *gameTimer;      // 游戏计时器
    int penaltySeconds = 0; // 总时间（包含实际时间和罚时）
    QString timeText;       // 时间显示文本
    int rings = 3;          // 游戏的环数
    int stepCount = 0;      // 记录步数
    int currentLevelId = 0;

    // 网络同步相关
    SocketManager *socketManager = nullptr;
    bool isServer = false;            // 是否是服务器, 由setOnlineMode设置
    OnlineChat *onlineChat = nullptr; // 联机聊天窗口
    bool isOnlineMode = false;        // 是否为联机模式，用于设置聊天窗口是否显示

    DataManager *dataManager;

    // 粒子系统相关
    QTimer *particleTimer;                        // 粒子更新定时器
    QTimer *effectTimer;                          // 特效状态转换定时器
    QMap<int, QVector<HexParticle>> hexParticles; // 每个六边形的粒子集合
    QMap<int, HexState> hexStates;                // 每个六边形的状态
    QMap<int, float> dissolveProgress;            // 每个六边形的分解进度
    QMap<int, float> reconstructProgress;         // 每个六边形的重构进度
    QMap<int, QVector<QPointF>> hexOutlinePoints; // 每个六边形的轮廓点
    int effectDuration = 400;                     // 特效持续时间(毫秒)
    const int frameInterval = 16;                 // 帧间隔(ms)，约60帧每秒
};

#endif // GAME_H
