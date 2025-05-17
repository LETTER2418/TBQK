#ifndef ANIMATION_H
#define ANIMATION_H

#include <QWidget>
#include <QTimer>
#include <QPainter>

class Animation : public QWidget {
    Q_OBJECT

public:
    explicit Animation(QWidget *parent = nullptr);
    ~Animation() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateRotation();

private:
    void drawTaiji(QPainter &painter);
    
    QTimer *timer;        // 用于动画更新的定时器
    qreal rotationAngle;  // 当前旋转角度
    QColor backgroundColor; // 背景颜色
    int radius;           // 太极图半径
    qreal rotationSpeed;  // 旋转速度 (度/帧)
};

#endif // ANIMATION_H 