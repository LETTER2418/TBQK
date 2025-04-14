#include "BUTTON.h"

BUTTON::BUTTON(const QString &text, QWidget *parent, const QString &soundFilePath)
    : QPushButton(text, parent), gradientOffset(-BUTTON_WIDTH) // 初始化流光偏移量
{
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

    // 连接按钮点击事件，播放音效
    connect(this, &QPushButton::clicked, player, &QMediaPlayer::play);

    // 创建定时器，用于定时更新流光偏移量
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BUTTON::updateGradient);
    timer->start(30); // 每30毫秒更新一次

    setStyleSheet("QPushButton {"
                  "background: rgba(255, 255, 255, 0);"
                  "border: none;"
                  "}"

                  "QPushButton:hover {"
                  "background: rgba(255, 255, 255, 0.2);"
                  "}");
}

void BUTTON::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

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

void BUTTON::updateGradient()
{
    gradientOffset += 3; // 每次移动3像素
    if (gradientOffset > width()) {
        gradientOffset = -BUTTON_WIDTH; // 重新循环
    }
    update(); // 触发重绘
}
