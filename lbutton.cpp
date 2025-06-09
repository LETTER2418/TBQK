#include "lbutton.h"
#include <QMouseEvent>
#include <cmath>

const QString Lbutton::soundFilePath = "../../sound/BUTTON.ogg";

// 初始化静态成员
QSet<Lbutton *> Lbutton::allButtons;
float Lbutton::globalVolume = 0.0f;

Lbutton::Lbutton(QWidget *parent, const QString &text)
    : QPushButton(text, parent),
      gradientOffset(-BUTTON_WIDTH), // 初始化流光偏移量
      particleEffectEnabled(true),   // 默认启用粒子特效
      clickEffectEnabled(false),     // 默认关闭点击分解重构特效
      buttonState(ButtonState::Normal),
      dissolveProgress(0.0f),
      reconstructProgress(0.0f)
{
    // 添加到全局按钮集合
    allButtons.insert(this);

    // 设置按钮的大小
    setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // 设置按钮的字体
    QFont buttonFont;
    buttonFont.setPointSize(FONT_SIZE);
    setFont(buttonFont);

    // 创建媒体播放器和音频输出
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(soundFilePath));

    // 设置初始音量
    if (audioOutput)
    {
        audioOutput->setVolume(globalVolume);
    }

    // 连接按钮点击事件，播放音效
    connect(this, &QPushButton::clicked, player, &QMediaPlayer::play);

    // 连接按钮点击事件到分解效果
    connect(this, &QPushButton::clicked, this, [this]()
            {
        // 延迟触发分解效果，让其他clicked连接的槽函数先执行
        QTimer::singleShot(50, this, &Lbutton::triggerDissolveEffect); });

    // 创建定时器，用于定时更新流光偏移量
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Lbutton::updateGradient);
    timer->start(30); // 每30毫秒更新一次

    // 创建特效状态转换定时器
    effectTimer = new QTimer(this);
    effectTimer->setSingleShot(true);

    // 初始化粒子系统
    initParticleSystem();

    // 生成按钮轮廓点
    generateButtonOutline();

    setStyleSheet("QPushButton {"
                  "background: rgba(255, 255, 255, 0);"
                  "border: none;"
                  "color:white"
                  "}"

                  "QPushButton:hover {"
                  "background: rgba(255, 255, 255, 0.2);"
                  "}");
}

Lbutton::Lbutton(QWidget *parent, const QString &text, QString color, int fontSize)
    : QPushButton(text, parent),
      gradientOffset(-BUTTON_WIDTH), // 初始化流光偏移量
      particleEffectEnabled(true),   // 默认启用粒子特效
      clickEffectEnabled(false),     // 默认关闭点击分解重构特效
      buttonState(ButtonState::Normal),
      dissolveProgress(0.0f),
      reconstructProgress(0.0f)
{
    // 添加到全局按钮集合
    allButtons.insert(this);

    // 设置按钮的大小
    setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // 设置按钮的字体
    QFont buttonFont;
    buttonFont.setPointSize(fontSize);
    setFont(buttonFont);

    // 创建媒体播放器和音频输出
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(soundFilePath));

    // 设置初始音量
    if (audioOutput)
    {
        audioOutput->setVolume(globalVolume);
    }

    // 连接按钮点击事件，播放音效
    connect(this, &QPushButton::clicked, player, &QMediaPlayer::play);

    // 连接按钮点击事件到分解效果
    connect(this, &QPushButton::clicked, this, [this]()
            {
        // 延迟触发分解效果，让其他clicked连接的槽函数先执行
        QTimer::singleShot(50, this, &Lbutton::triggerDissolveEffect); });

    // 创建定时器，用于定时更新流光偏移量
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Lbutton::updateGradient);
    timer->start(30); // 每30毫秒更新一次

    // 创建特效状态转换定时器
    effectTimer = new QTimer(this);
    effectTimer->setSingleShot(true);

    // 初始化粒子系统
    initParticleSystem();

    // 生成按钮轮廓点
    generateButtonOutline();

    QString style = QString(R"(
    QPushButton {
        background: rgba(255, 255, 255, 0);
        border: none;
        color: %1;
    }

    QPushButton:hover {
        background: rgba(255, 255, 255, 0.2);
    }
)")
                        .arg(color);

    setStyleSheet(style);
}

Lbutton::~Lbutton()
{
    // 从全局按钮集合中移除
    allButtons.remove(this);
}

void Lbutton::paintEvent(QPaintEvent *event)
{
    // 如果按钮处于分解或重构状态，不调用基类的绘制方法
    if (buttonState == ButtonState::Normal)
    {
        QPushButton::paintEvent(event);
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 仅在正常状态下绘制流光边框
    if (buttonState == ButtonState::Normal)
    {
        // 创建线性渐变（比按钮宽2倍，形成流光移动效果）
        QLinearGradient gradient(gradientOffset, 0, gradientOffset + BUTTON_WIDTH * 2, 0);
        gradient.setColorAt(0.0, QColor(255, 100, 100, 200)); // 浅红色
        gradient.setColorAt(0.2, QColor(100, 255, 100, 200)); // 浅绿色
        gradient.setColorAt(0.4, QColor(100, 100, 255, 200)); // 浅蓝色
        gradient.setColorAt(0.6, QColor(255, 255, 100, 200)); // 浅黄色
        gradient.setColorAt(0.8, QColor(255, 100, 255, 200)); // 浅洋红色

        // 设置画笔
        QPen pen;
        pen.setWidth(5); // 边框宽度
        pen.setBrush(gradient);
        painter.setPen(pen);

        // 绘制带渐变的圆角矩形边框
        painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 10, 10);
    }

    // 绘制粒子
    if (particleEffectEnabled)
    {
        drawParticles(painter);
    }
}

void Lbutton::updateGradient()
{
    gradientOffset += 3; // 每次移动3像素
    if (gradientOffset > width())
    {
        gradientOffset = -BUTTON_WIDTH; // 重新循环
    }
    update(); // 触发重绘
}

// 设置所有按钮的音量
void Lbutton::setGlobalButtonVolume(float volume)
{
    globalVolume = volume;
    for (Lbutton *button : allButtons)
    {
        if (button && button->audioOutput)
        {
            button->audioOutput->setVolume(volume);
        }
    }
}

// 初始化粒子系统
void Lbutton::initParticleSystem()
{
    // 创建粒子更新定时器
    particleTimer = new QTimer(this);
    connect(particleTimer, &QTimer::timeout, this, &Lbutton::updateParticles);
    particleTimer->start(frameInterval); // 约60帧每秒

    // 初始化粒子容器
    particles.reserve(500); // 预分配空间以提高性能
}

// 更新粒子系统
void Lbutton::updateParticles()
{
    // 根据按钮状态更新粒子
    if (buttonState == ButtonState::Dissolving)
    {
        updateDissolveEffect();
    }
    else if (buttonState == ButtonState::Reconstructing)
    {
        updateReconstructEffect();
    }
    else
    {
        // 正常状态下更新粒子
        for (int i = 0; i < particles.size(); ++i)
        {
            Particle &p = particles[i];

            // 更新生命周期
            p.life -= 0.01f;

            // 移除已经死亡的粒子
            if (p.life <= 0)
            {
                particles.remove(i);
                --i;
                continue;
            }

            // 更新位置
            p.pos += p.velocity;

            // 更新透明度（随生命周期减少）
            p.alpha = p.life / p.maxLife * 255;

            // 添加一些随机性使粒子运动更自然
            p.velocity += QPointF(
                (QRandomGenerator::global()->generateDouble() * 0.2) - 0.1,
                (QRandomGenerator::global()->generateDouble() * 0.2) - 0.1);

            // 重力效果
            p.velocity.setY(p.velocity.y() + 0.01f);
        }
    }

    // 需要重绘
    update();
}

// 在指定位置创建粒子
void Lbutton::createParticles(const QPointF &pos, int count, float initialSpeed)
{
    if (!particleEffectEnabled || buttonState != ButtonState::Normal)
        return;

    for (int i = 0; i < count; ++i)
    {
        Particle p;
        p.pos = pos;
        p.startPos = pos;
        p.targetPos = pos;
        p.isReconstructing = false;

        // 随机方向
        double angle = QRandomGenerator::global()->generateDouble() * 2.0 * M_PI; // 0-2π
        double speedMultiplier = 0.5 + QRandomGenerator::global()->generateDouble();

        p.velocity = QPointF(
            cos(angle) * initialSpeed * speedMultiplier,
            sin(angle) * initialSpeed * speedMultiplier);

        // 随机颜色（使用按钮的彩色渐变色）
        double colorPos = QRandomGenerator::global()->generateDouble();
        if (colorPos < 0.2)
            p.color = QColor(255, 100, 100); // 浅红色
        else if (colorPos < 0.4)
            p.color = QColor(100, 255, 100); // 浅绿色
        else if (colorPos < 0.6)
            p.color = QColor(100, 100, 255); // 浅蓝色
        else if (colorPos < 0.8)
            p.color = QColor(255, 255, 100); // 浅黄色
        else
            p.color = QColor(255, 100, 255); // 浅洋红色

        // 其他属性
        p.size = 1 + QRandomGenerator::global()->bounded(4);               // 1-4的整数
        p.life = 0.3 + QRandomGenerator::global()->generateDouble() * 0.7; // 0.3-1.0之间
        p.maxLife = p.life;
        p.alpha = 255;

        // 添加到粒子容器
        particles.append(p);
    }
}

// 绘制所有粒子
void Lbutton::drawParticles(QPainter &painter)
{
    painter.save();

    // 使用混合模式使粒子更亮
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // 绘制每个粒子
    for (const Particle &p : particles)
    {
        QColor color = p.color;
        color.setAlpha(p.alpha);

        painter.setPen(Qt::NoPen);
        painter.setBrush(color);

        // 绘制粒子（圆形）
        painter.drawEllipse(p.pos, p.size, p.size);
    }

    painter.restore();
}

// 启用或禁用粒子特效
void Lbutton::enableParticleEffect(bool enable)
{
    particleEffectEnabled = enable;

    // 如果禁用，清除所有现有粒子
    if (!enable)
    {
        particles.clear();
    }
}

// 鼠标进入事件
void Lbutton::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);

    // 如果不在正常状态，不创建进入特效
    if (buttonState != ButtonState::Normal)
        return;

    // 鼠标进入时创建一些粒子
    QPointF center(width() / 2.0, height() / 2.0);
    createParticles(center, 20, 1.0f);
}

// 鼠标离开事件
void Lbutton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
}

// 鼠标按下事件
void Lbutton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);

    // 如果不在正常状态，不处理按下事件
    if (buttonState != ButtonState::Normal)
        return;

    // 点击时在鼠标位置创建爆发效果
    createParticles(event->position(), 30, 2.0f);
}

// 触发按钮分解效果
void Lbutton::triggerDissolveEffect()
{
    // 如果特效未启用或点击分解重构特效关闭，直接返回
    if (!particleEffectEnabled || !clickEffectEnabled)
        return;

    // 如果已经在特效状态中，先重置为正常状态
    if (buttonState != ButtonState::Normal)
    {
        buttonState = ButtonState::Normal;

        // 如果定时器正在运行，停止它
        if (effectTimer->isActive())
        {
            effectTimer->stop();
        }

        // 断开所有可能的连接
        disconnect(effectTimer, &QTimer::timeout, this, &Lbutton::onDissolveFinished);
        disconnect(effectTimer, &QTimer::timeout, this, &Lbutton::startReconstruction);
        disconnect(effectTimer, &QTimer::timeout, this, &Lbutton::onReconstructFinished);
    }

    // 清除现有粒子
    particles.clear();

    // 创建分解粒子
    createDissolveParticles();

    // 设置按钮状态为分解中
    buttonState = ButtonState::Dissolving;
    dissolveProgress = 0.0f;

    // 设置定时器，在分解完成后触发回调
    connect(effectTimer, &QTimer::timeout, this, &Lbutton::onDissolveFinished);
    effectTimer->start(effectDuration);

    // 强制更新，确保立即显示粒子效果
    update();
}

// 分解完成处理
void Lbutton::onDissolveFinished()
{
    // 断开定时器信号连接
    disconnect(effectTimer, &QTimer::timeout, this, &Lbutton::onDissolveFinished);

    // 设置按钮状态为已分解
    buttonState = ButtonState::Dissolved;

    // 短暂停留在分解状态，然后开始重构
    connect(effectTimer, &QTimer::timeout, this, &Lbutton::startReconstruction);
    effectTimer->start(0); // 等待0毫秒后开始重构
}

// 开始重构
void Lbutton::startReconstruction()
{
    // 断开定时器信号连接
    disconnect(effectTimer, &QTimer::timeout, this, &Lbutton::startReconstruction);

    // 设置按钮状态为重构中
    buttonState = ButtonState::Reconstructing;
    reconstructProgress = 0.0f;

    // 为粒子设置目标位置（按钮轮廓）
    for (int i = 0; i < particles.size(); ++i)
    {
        Particle &p = particles[i];
        // 将粒子标记为重构状态
        p.isReconstructing = true;
        // 记录起始位置
        p.startPos = p.pos;
        // 设置目标位置（按钮轮廓点或内部随机点）
        if (i < buttonOutlinePoints.size())
        {
            p.targetPos = buttonOutlinePoints[i];
        }
        else
        {
            // 对于多余的粒子，使用按钮内部的随机点
            p.targetPos = QPointF(
                QRandomGenerator::global()->bounded(10, width() - 10),
                QRandomGenerator::global()->bounded(10, height() - 10));
        }
    }

    // 设置定时器，在重构完成后触发回调
    connect(effectTimer, &QTimer::timeout, this, &Lbutton::onReconstructFinished);
    effectTimer->start(effectDuration);
}

// 重构完成处理
void Lbutton::onReconstructFinished()
{
    // 断开定时器信号连接
    disconnect(effectTimer, &QTimer::timeout, this, &Lbutton::onReconstructFinished);

    // 设置按钮状态为正常
    buttonState = ButtonState::Normal;

    // 清除所有粒子
    particles.clear();

    // 创建一些完成效果的粒子
    QPointF center(width() / 2.0, height() / 2.0);
    createParticles(center, 50, 1.5f);
}

// 生成按钮轮廓点
void Lbutton::generateButtonOutline()
{
    buttonOutlinePoints.clear();
    const int numPoints = 200; // 轮廓点数量

    // 计算圆角矩形的轮廓点
    QRect buttonRect = rect().adjusted(5, 5, -5, -5);
    int cornerRadius = 10;

    // 计算每个边的点数
    int pointsPerSide = numPoints / 4;

    // 上边
    for (int i = 0; i < pointsPerSide; ++i)
    {
        float t = (float)i / pointsPerSide;
        float x = buttonRect.left() + cornerRadius + t * (buttonRect.width() - 2 * cornerRadius);
        buttonOutlinePoints.append(QPointF(x, buttonRect.top()));
    }

    // 右边
    for (int i = 0; i < pointsPerSide; ++i)
    {
        float t = (float)i / pointsPerSide;
        float y = buttonRect.top() + cornerRadius + t * (buttonRect.height() - 2 * cornerRadius);
        buttonOutlinePoints.append(QPointF(buttonRect.right(), y));
    }

    // 下边
    for (int i = 0; i < pointsPerSide; ++i)
    {
        float t = (float)i / pointsPerSide;
        float x = buttonRect.right() - cornerRadius - t * (buttonRect.width() - 2 * cornerRadius);
        buttonOutlinePoints.append(QPointF(x, buttonRect.bottom()));
    }

    // 左边
    for (int i = 0; i < pointsPerSide; ++i)
    {
        float t = (float)i / pointsPerSide;
        float y = buttonRect.bottom() - cornerRadius - t * (buttonRect.height() - 2 * cornerRadius);
        buttonOutlinePoints.append(QPointF(buttonRect.left(), y));
    }

    // 添加圆角部分的点
    // 这里简化处理，实际可以使用更精确的圆弧计算
}

// 创建分解粒子
void Lbutton::createDissolveParticles()
{
    // 为按钮的轮廓和内部创建粒子
    const int totalParticles = 300; // 总粒子数

    for (int i = 0; i < totalParticles; ++i)
    {
        Particle p;

        // 如果有足够的轮廓点，从轮廓开始
        if (i < buttonOutlinePoints.size())
        {
            p.pos = buttonOutlinePoints[i];
        }
        else
        {
            // 否则在按钮区域内随机创建粒子
            p.pos = QPointF(
                QRandomGenerator::global()->bounded(5, width() - 5),
                QRandomGenerator::global()->bounded(5, height() - 5));
        }

        p.startPos = p.pos;
        p.targetPos = getRandomPointAround(p.pos, 100.0f); // 分解目标位置
        p.isReconstructing = false;

        // 随机初始速度（方向向外）
        QPointF direction = p.targetPos - p.pos;
        float length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0)
        {
            direction = QPointF(direction.x() / length, direction.y() / length);
        }
        else
        {
            direction = QPointF(0, 0);
        }

        float speed = 0.5f + QRandomGenerator::global()->generateDouble() * 1.5f;
        p.velocity = QPointF(direction.x() * speed, direction.y() * speed);

        // 随机颜色（使用按钮的彩色渐变色）
        double colorPos = QRandomGenerator::global()->generateDouble();
        if (colorPos < 0.2)
            p.color = QColor(255, 100, 100); // 浅红色
        else if (colorPos < 0.4)
            p.color = QColor(100, 255, 100); // 浅绿色
        else if (colorPos < 0.6)
            p.color = QColor(100, 100, 255); // 浅蓝色
        else if (colorPos < 0.8)
            p.color = QColor(255, 255, 100); // 浅黄色
        else
            p.color = QColor(255, 100, 255); // 浅洋红色

        // 其他属性
        p.size = 1 + QRandomGenerator::global()->bounded(3); // 1-3的整数
        p.life = 1.0f;                                       // 足够长的生命周期
        p.maxLife = p.life;
        p.alpha = 255;

        // 添加到粒子容器
        particles.append(p);
    }
}

// 更新分解效果
void Lbutton::updateDissolveEffect()
{
    dissolveProgress += 1.0f / (effectDuration / static_cast<float>(frameInterval)); // 每帧更新进度
    dissolveProgress = qMin(dissolveProgress, 1.0f);

    for (int i = 0; i < particles.size(); ++i)
    {
        Particle &p = particles[i];

        // 计算当前位置（线性插值）
        p.pos = p.startPos + (p.targetPos - p.startPos) * dissolveProgress;

        // 添加一些随机性
        p.pos += QPointF(
            (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * dissolveProgress * 2.0,
            (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * dissolveProgress * 2.0);
    }
}

// 更新重构效果
void Lbutton::updateReconstructEffect()
{
    reconstructProgress += 1.0f / (effectDuration / static_cast<float>(frameInterval)); // 每帧更新进度
    reconstructProgress = qMin(reconstructProgress, 1.0f);

    float easedProgress = 1.0f - pow(1.0f - reconstructProgress, 3.0f); // 缓动函数

    for (int i = 0; i < particles.size(); ++i)
    {
        Particle &p = particles[i];

        // 计算当前位置（使用缓动函数）
        p.pos = p.startPos + (p.targetPos - p.startPos) * easedProgress;

        // 随着接近目标位置减少随机性
        float randomFactor = 1.0f - easedProgress;
        p.pos += QPointF(
            (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * randomFactor * 2.0,
            (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * randomFactor * 2.0);
    }
}

// 获取周围的随机点
QPointF Lbutton::getRandomPointAround(const QPointF &center, float radius)
{
    // 生成随机角度
    double angle = QRandomGenerator::global()->generateDouble() * 2.0 * M_PI;

    // 生成随机距离（使用平方根使分布均匀）
    double distance = radius * sqrt(QRandomGenerator::global()->generateDouble());

    // 计算新位置
    return QPointF(
        center.x() + cos(angle) * distance,
        center.y() + sin(angle) * distance);
}

// 启用或禁用点击分解重构特效
void Lbutton::enableClickEffect(bool enable)
{
    clickEffectEnabled = enable;
}
