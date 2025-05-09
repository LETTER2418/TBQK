#include "widget.h"
#include "ui_widget.h"

#include <QPainter>
#include <QPixmap>
#include <QMovie>
#include <QLabel>
#include <QDebug>
#include <QPaintEvent>
#include <QResizeEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("提笔乾坤");
    this->resize(1705, 882);
    this->move(0, 0);

    // 设置背景图片
    setBackgroundImage();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setBackgroundImage()
{
    backgroundImage = QPixmap(backgroundImagePath);

    if (backgroundImage.isNull()) {
        qDebug() << "背景图片加载失败";
        return;
    }

    update(); // 触发重绘
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull()) {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
