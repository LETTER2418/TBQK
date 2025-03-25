#include "start.h"
#include "button.h"

start::start(Widget *parent) : Widget(parent), stack(new QStackedWidget(this))

{
    // backbutton = new button("返回", this);
    // backbutton->move(0, 0);

    // 创建四个按钮
    startButton = new button("开始游戏", this);
    aboutButton = new button("关于", this);
    settingsButton = new button("设置", this);
    exitButton = new button("退出", this);


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


    //管理多个子窗口的显示
    // 创建页面
    QWidget *page1 = new QWidget();
    QWidget *page2 = new QWidget();
    QWidget *page3 = new QWidget();

    // 创建页面内容
    QLabel *label1 = new QLabel("This is Page 1", page1);
    QLabel *label2 = new QLabel("This is Page 2", page2);
    QLabel *label3 = new QLabel("This is Page 3", page3);

    // 创建切换页面的按钮
    QPushButton *btnPage1 = new QPushButton("Go to Page 1", this);
    QPushButton *btnPage2 = new QPushButton("Go to Page 2", this);
    QPushButton *btnPage3 = new QPushButton("Go to Page 3", this);

    // 将标签和按钮添加到每个页面
    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    layout1->addWidget(label1);
    layout1->addWidget(btnPage1);

    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    layout2->addWidget(label2);
    layout2->addWidget(btnPage2);

    QVBoxLayout *layout3 = new QVBoxLayout(page3);
    layout3->addWidget(label3);
    layout3->addWidget(btnPage3);

    // 将页面添加到 QStackedWidget
    stack->addWidget(page1);
    stack->addWidget(page2);
    stack->addWidget(page3);

    // 设置默认显示的页面
    stack->setCurrentIndex(0);

    // 设置切换按钮的功能
    connect(btnPage1, &QPushButton::clicked, [this]() {
        this->stack->setCurrentIndex(1);
    });
    connect(btnPage2, &QPushButton::clicked, [this]() {
        this->stack->setCurrentIndex(2);
    });
    connect(btnPage3, &QPushButton::clicked, [this]() {
        this->stack->setCurrentIndex(0);
    });

    // 布局：将 QStackedWidget 放入主窗口
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stack);
    this->setLayout(mainLayout);
}

start::~start() {
    delete startButton;
    delete settingsButton;
    delete aboutButton;
    delete exitButton;
}
