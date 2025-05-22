#include "animation.h"
#include <QResizeEvent>
#include <QPainterPath>
#include <QShowEvent>

Animation::Animation(QWidget *parent)
    : QWidget(parent), rotationAngle(0), backgroundColor(Qt::lightGray), radius(100), rotationSpeed(2.0)
{
    // 设置窗口属性
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setWindowTitle("求解中(●′ω`●) ");
    resize(300, 400);

    // 设置窗口居中显示
    if (parent)
    {
        // 计算窗口应该显示的位置，使其在父窗口中居中
        int x = parent->geometry().center().x() - width() / 2;
        int y = parent->geometry().center().y() - height() / 2;
        move(x, y);
    }

    // 初始化定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Animation::updateRotation);
    timer->start(30); // 约33帧/秒

    // 设置背景
    // rgb(255,255,255)
    backgroundColor = QColor(255, 255, 255);

    // 设置旋转速度 (度/帧)
    rotationSpeed = 3;
}

Animation::~Animation()
{
    if (timer)
    {
        timer->stop();
    }
}

void Animation::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), backgroundColor);

    // 移动到窗口中心
    painter.translate(width() / 2, height() / 2);

    // 应用旋转
    painter.rotate(rotationAngle);

    // 绘制太极图
    drawTaiji(painter);
}

void Animation::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // 调整太极图的大小，使其适应窗口
    radius = qMin(width(), height()) / 3;
}

void Animation::updateRotation()
{
    // 更新旋转角度
    rotationAngle += rotationSpeed;
    if (rotationAngle >= 360.0)
    {
        rotationAngle -= 360.0;
    }

    // 触发重绘
    update();
}

void Animation::drawTaiji(QPainter &painter)
{
    // 设置画笔
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    // 绘制太极外圆
    painter.drawEllipse(-radius, -radius, 2 * radius, 2 * radius);

    // 使用更简单的方法绘制太极图
    painter.setPen(Qt::NoPen);

    // 绘制黑色半圆
    painter.setBrush(Qt::black);
    painter.drawPie(-radius, -radius, 2 * radius, 2 * radius, 90 * 16, 180 * 16);

    // 绘制白色半圆
    painter.setBrush(Qt::white);
    painter.drawPie(-radius, -radius, 2 * radius, 2 * radius, 270 * 16, 180 * 16);

    // 绘制黑白相互嵌入的两个小圆
    int halfRadius = radius / 2;

    // 上部白底黑圆
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(0, -halfRadius), halfRadius, halfRadius);

    // 下部黑底白圆
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, halfRadius), halfRadius, halfRadius);

    // 绘制小圆点
    int smallRadius = radius / 6;

    // 上部黑圆中的白点
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, -halfRadius), smallRadius, smallRadius);

    // 下部白圆中的黑点
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(0, halfRadius), smallRadius, smallRadius);

    // 恢复画笔
    painter.setPen(pen);
}

void Animation::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 在每次显示窗口时确保窗口居中
    if (parentWidget())
    {
        // 计算窗口应该显示的位置，使其在父窗口中居中
        int x = parentWidget()->geometry().center().x() - width() / 2;
        int y = parentWidget()->geometry().center().y() - height() / 2;
        move(x, y);
    }
}