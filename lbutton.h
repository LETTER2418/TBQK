#ifndef LBUTTON_H
#define LBUTTON_H

#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPainter>
#include <QTimer>
#include <QLinearGradient>
#include <QFont>
#include <QVector>
#include <QPointF>
#include <QColor>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>

const int BUTTON_WIDTH = 140;
const int BUTTON_HEIGHT = 50;
const int FONT_SIZE = 14;

// 粒子结构体
struct Particle {
    QPointF pos;         // 当前位置
    QPointF velocity;    // 移动速度
    QPointF startPos;    // 起始位置(重构用)
    QPointF targetPos;   // 目标位置(重构用)
    QColor color;        // 颜色
    float size;          // 大小
    float alpha;         // 透明度
    float life;          // 生命周期
    float maxLife;       // 最大生命周期
    bool isReconstructing; // 是否处于重构状态
};

// 按钮状态枚举
enum class ButtonState {
    Normal,       // 正常状态
    Dissolving,   // 分解中
    Dissolved,    // 已分解
    Reconstructing // 重构中
};

class Lbutton : public QPushButton
{
    Q_OBJECT

public:
    // 构造函数，接收按钮文字
    explicit Lbutton(QWidget *parent = nullptr, const QString &text="Lbutton");
    explicit Lbutton(QWidget *parent, const QString &text, QString color, int fontSize=12);
    
    // 设置按钮音量
    void setButtonVolume(float volume);
    
    // 设为public成员，以便外部访问
    QAudioOutput *audioOutput; // 音频输出

    // 粒子特效开关
    void enableParticleEffect(bool enable);
    
    // 控制点击分解重构特效的开关
    void enableClickEffect(bool enable);
    
    // 触发按钮分解效果
    void triggerDissolveEffect();

protected:
    // 重写paintEvent实现自定义绘制
    void paintEvent(QPaintEvent *event) override;
    // 鼠标进入事件
    void enterEvent(QEnterEvent *event) override;
    // 鼠标离开事件
    void leaveEvent(QEvent *event) override;
    // 鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    // 更新渐变效果
    void updateGradient();
    // 更新粒子系统
    void updateParticles();
    // 分解完成处理
    void onDissolveFinished();
    // 开始重构
    void startReconstruction();
    // 重构完成处理
    void onReconstructFinished();

signals:
    // 定义自己的clicked信号，以便在需要时发出
    void myClicked();

private:
    QMediaPlayer *player;     // 用于播放按钮点击音效
    QTimer *timer;            // 定时器，用于更新流光效果
    QTimer *particleTimer;    // 粒子系统更新定时器
    QTimer *effectTimer;      // 特效状态转换定时器
    int gradientOffset;       // 流光偏移量
    static const QString soundFilePath;

    // 粒子系统相关
    QVector<Particle> particles;           // 粒子容器
    QVector<QPointF> buttonOutlinePoints;  // 按钮轮廓点
    bool particleEffectEnabled;            // 粒子特效开关
    bool clickEffectEnabled;               // 点击分解重构特效开关
    ButtonState buttonState;               // 按钮当前状态
    float dissolveProgress;                // 分解进度
    float reconstructProgress;             // 重构进度
    int effectDuration=400;                    // 分解/重构效果持续时间(毫秒)
    
    // 粒子系统方法
    void initParticleSystem();
    void createParticles(const QPointF &pos, int count, float initialSpeed = 1.0f);
    void drawParticles(QPainter &painter);
    
    // 分解重构相关方法
    void generateButtonOutline();
    void createDissolveParticles();
    void updateDissolveEffect();
    void updateReconstructEffect();
    QPointF getRandomPointAround(const QPointF &center, float radius);
};

#endif // LBUTTON_H
