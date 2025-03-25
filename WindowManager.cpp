#include "WindowManager.h"
#include "button.h"
#include "start.h"
#include "about.h"

WindowManager::WindowManager(Widget *parent) : Widget(parent), stack(new QStackedWidget(this))

{
    // backbutton = new button("返回", this);
    // backbutton->move(0, 0);

    // 创建 page1，并将四个按钮放入
    QWidget *page1 = new QWidget();

    startButton = new button("开始游戏", page1);
    aboutButton = new button("关于", page1);
    settingsButton = new button("设置", page1);
    exitButton = new button("退出", page1);

    // 退出按钮连接到窗口关闭
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(startButton, 0, 0);
    layout->addWidget(aboutButton, 1, 0);
    layout->addWidget(settingsButton, 0, 1);
    layout->addWidget(exitButton, 1, 1);

    // **创建一个 QWidget 作为中央部件**
    QWidget *buttonWidget = new QWidget(page1);
    buttonWidget->setLayout(layout);
    buttonWidget->setFixedSize(400, 200);  // 固定大小

    // **创建一个布局让 buttonWidget 居中**
    QVBoxLayout *page1Layout = new QVBoxLayout(page1);
    page1Layout->addStretch();   // 上方弹性空间
    page1Layout->addWidget(buttonWidget, 0, Qt::AlignCenter);
    page1Layout->addStretch();   // 下方弹性空间
    page1->setLayout(page1Layout);

    start *page2 = new start();
    connect(page2->backButton, &QPushButton::clicked, [this, page1]() {
        this->stack->setCurrentWidget(page1);
    });

    about *page3 = new about();
    connect(page3->backButton, &QPushButton::clicked, [this, page1]() {
        this->stack->setCurrentWidget(page1);
    });

    connect(startButton, &QPushButton::clicked, [this,page2]() {
        this->stack->setCurrentWidget(page2);
    });
    connect(aboutButton, &QPushButton::clicked, [page3,this]() {
        this->stack->setCurrentWidget(page3);
    });

    // 将页面添加到 QStackedWidget
    stack->addWidget(page1);
    stack->addWidget(page2);
    stack->addWidget(page3);

    // 设置默认显示的页面
    stack->setCurrentIndex(0);

    // 布局：将 QStackedWidget 放入主窗口
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stack);
    this->setLayout(mainLayout);
}

WindowManager::~WindowManager() {
    delete startButton;
    delete settingsButton;
    delete aboutButton;
    delete exitButton;
}
