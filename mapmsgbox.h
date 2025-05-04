#ifndef MAPMSGBOX_H
#define MAPMSGBOX_H

#include <QWidget>
#include <QColorDialog>
#include <QLabel>
#include <QComboBox>
#include "lbutton.h"
#include <QPainter>

class MapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit MapMsgBox(QWidget *parent = nullptr);
    ~MapMsgBox();
    int rings;
    QColor color1, color2, color3;

signals:
    void sendMsg(int, QColor, QColor, QColor);

private slots:
    void openColorDialog1();
    void openColorDialog2();
    void openColorDialog3();

private:
    void paintEvent(QPaintEvent *event) override;
    void setupUI();

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
};

#endif // MAPMSGBOX_H
