#include "widget.h"
#include "ui_widget.h"

#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("提笔乾坤");
    this->resize(1705, 882);
    this->move(0, 0);

    // 创建 QLabel 用于显示背景图片
    QLabel *bgLabel = new QLabel(this);
    QPixmap bgImage(":\\image\\bg1.jpg");

    // 检查图片是否加载成功
    if (bgImage.isNull())
    {
        qDebug() << "图片加载失败，请检查路径！";
        return; // 退出程序
    }

    // 设置 QLabel 填充整个窗口
    bgLabel->setPixmap(bgImage);
    bgLabel->setScaledContents(true);
    bgLabel->setGeometry(0, 0, this->width(), this->height());
}

Widget::~Widget()
{
    delete ui;
}
