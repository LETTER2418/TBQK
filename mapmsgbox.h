#ifndef MAPMSGBOX_H
#define MAPMSGBOX_H

#include <QWidget>
#include <QColorDialog>
#include <QLabel>
#include <QComboBox>
#include "lbutton.h"
#include <QPainter>
#include <QShowEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class MapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit MapMsgBox(QWidget *parent = nullptr);
    ~MapMsgBox();
    int rings;
    QColor color1, color2, color3;

    void show(); // 重写show方法以支持动画

signals:
    void sendMsg(int, QColor, QColor, QColor);

private slots:
    void openColorDialog1();
    void openColorDialog2();
    void openColorDialog3();

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void setupUI();
    void setupFadeAnimation(); // 设置透明度动画

    QPixmap backgroundImage;
    const QString backgroundImagePath = ":\\image\\msg.png"; // 存储背景图片路径

    // 控件
    Lbutton *closeButton;
    Lbutton *color1Button;
    Lbutton *color2Button;
    Lbutton *color3Button;
    Lbutton *radiusButton;
    QLabel *color1Label;
    QLabel *color2Label;
    QLabel *color3Label;
    QComboBox *comboBox;

    // 透明度相关
    QGraphicsOpacityEffect *opacityEffect;
    QPropertyAnimation *fadeAnimation;
    bool hasUsedFadeAnimation = false;
};

#endif // MAPMSGBOX_H
