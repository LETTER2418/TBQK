#include "WindowManager.h"
#include "BUTTON.h"
#include "start.h"
#include "about.h"
#include "menu.h"

WindowManager::WindowManager(Widget *parent) : Widget(parent), stack(new QStackedWidget(this))
{

    // 创建 mainPage，并将四个按钮放入
    QWidget *mainPage = new QWidget();

    startButton = new BUTTON("开始游戏", mainPage);
    aboutButton = new BUTTON("关于", mainPage);
    settingsButton = new BUTTON("设置", mainPage);
    exitButton = new BUTTON("退出", mainPage);

    // 退出按钮连接到窗口关闭
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(startButton, 0, 0);
    layout->addWidget(aboutButton, 1, 0);
    layout->addWidget(settingsButton, 0, 1);
    layout->addWidget(exitButton, 1, 1);

    // **创建一个 QWidget 作为中央部件**
    QWidget *buttonWidget = new QWidget(mainPage);
    buttonWidget->setLayout(layout);
    buttonWidget->setFixedSize(400, 200);  // 固定大小

    // **创建一个布局让 buttonWidget 居中**
    QVBoxLayout *page1Layout = new QVBoxLayout(mainPage);
    page1Layout->addStretch();   // 上方弹性空间
    page1Layout->addWidget(buttonWidget, 0, Qt::AlignCenter);
    page1Layout->addStretch();   // 下方弹性空间
    mainPage->setLayout(page1Layout);

    start *startPage = new start();
    about *aboutPage = new about();
    menu *menuPage = new menu();

    connect(startPage->backButton, &QPushButton::clicked, [this, mainPage]() {
        this->stack->setCurrentWidget(mainPage);
    });

    connect(aboutPage->backButton, &QPushButton::clicked, [this, mainPage]() {
        this->stack->setCurrentWidget(mainPage);
    });

    connect(startButton, &QPushButton::clicked, [this,startPage]() {
        this->stack->setCurrentWidget(startPage);
    });

    connect(aboutButton, &QPushButton::clicked, [this, aboutPage]() {
        this->stack->setCurrentWidget(aboutPage);
    });

    connect(startPage->messageBox->closeButton, &QPushButton::clicked, [this,menuPage,startPage]() {
        startPage->messageBox->accept();
        qDebug() << "MsgBox\n";
        this->stack->setCurrentWidget(menuPage);
    });

    // 将页面添加到 QStackedWidget
    stack->addWidget(mainPage);
    stack->addWidget(startPage);
    stack->addWidget(aboutPage);
    stack->addWidget(menuPage);

    // 设置默认显示的页面
    this->stack->setCurrentWidget(mainPage);

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
