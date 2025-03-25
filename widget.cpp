#include <QLabel>
#include <QApplication>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QGridLayout>
#include <QFont>
#include <widget.h>
#include <QStackedWidget>
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("Title");
    this->resize(1705, 882);
    this->move(0, 0);

    // 创建 QLabel 用于显示背景图片
    QLabel *bgLabel = new QLabel(this);
    QPixmap bgImage(":\\image\\bg.jpg");

    //检查图片是否加载成功
    if (bgImage.isNull()) {
        qDebug() << "图片加载失败，请检查路径！";
        return ; // 退出程序
    }

    //设置 QLabel 填充整个窗口
    bgLabel->setPixmap(bgImage);
    bgLabel->setScaledContents(true);
    bgLabel->setGeometry(0, 0, this->width(), this->height());

    // 创建四个按钮
    button *startButton = new button("开始游戏");
    button *aboutButton = new button("关于");
    button *settingsButton = new button("设置");
    button *exitButton = new button("退出");


    // 连接退出按钮的点击信号到窗口的关闭槽函数
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 创建网格布局管理器
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(startButton, 0, 0);
    layout->addWidget(aboutButton, 1, 0);
    layout->addWidget(settingsButton, 0, 1);
    layout->addWidget(exitButton, 1, 1);

    // 创建一个 QWidget 作为中央部件
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setLayout(layout);

    // 调整按钮布局，使其居中
    buttonWidget->setGeometry((this->width() - 400) / 2, (this->height() - 200) / 2, 400, 200);
}

Widget::~Widget()
{
    delete ui;
}
