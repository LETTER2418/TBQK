#include "WindowManager.h"
#include "Lbutton.h"
#include "Start.h"
#include "About.h"
#include "Menu.h"
#include "LevelEditor.h"
#include "RandomMap.h"

WindowManager::WindowManager(Widget *parent) : Widget(parent), pageStack(new QStackedWidget(this))
{

    // 创建 mainPage，并将四个按钮放入
    QWidget *mainPage = new QWidget(this);

    startButton = new Lbutton("开始游戏", mainPage);
    aboutButton = new Lbutton("关于", mainPage);
    settingsButton = new Lbutton("设置", mainPage);
    exitButton = new Lbutton("退出", mainPage);

    // 退出按钮连接到窗口关闭
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout(this);
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

    Start *startPage = new Start(this);
    About *aboutPage = new About(this);
    Menu *menuPage = new Menu(this);
    LevelEditor *levelEditorPage = new LevelEditor(this);
    RandomMap *randomMapPage =new RandomMap(this);

    connect(startPage->backButton, &QPushButton::clicked, [this, mainPage]() {
        this->pageStack->setCurrentWidget(mainPage);
    });

    connect(aboutPage->backButton, &QPushButton::clicked, [this, mainPage]() {
        this->pageStack->setCurrentWidget(mainPage);
    });

    connect(startButton, &QPushButton::clicked, [this,startPage]() {
        this->pageStack->setCurrentWidget(startPage);
    });

    connect(aboutButton, &QPushButton::clicked, [this, aboutPage]() {
        this->pageStack->setCurrentWidget(aboutPage);
    });

    connect(startPage->YESmessageBox->closeButton, &QPushButton::clicked, [this,menuPage,startPage]() {
        startPage->YESmessageBox->accept();
        this->pageStack->setCurrentWidget(menuPage);
    });

    connect(menuPage->logoutButton, &QPushButton::clicked, [this,startPage](){
        this->pageStack->setCurrentWidget(startPage);
    });

    connect(menuPage->levelEditorButton, &QPushButton::clicked, [this,levelEditorPage](){
            this->pageStack->setCurrentWidget(levelEditorPage);
    });

    connect(levelEditorPage->backButton,&QPushButton::clicked, [this,menuPage](){
        this->pageStack->setCurrentWidget(menuPage);
    });

    connect(levelEditorPage->randomButton,&QPushButton::clicked, [this,randomMapPage](){
        this->pageStack->setCurrentWidget(randomMapPage);
    });

    connect(randomMapPage->backButton,&QPushButton::clicked, [this,levelEditorPage](){
        this->pageStack->setCurrentWidget(levelEditorPage);
    });

    // 将页面添加到 QStackedWidget
    pageStack->addWidget(mainPage);
    pageStack->addWidget(startPage);
    pageStack->addWidget(aboutPage);
    pageStack->addWidget(menuPage);
    pageStack->addWidget(levelEditorPage);
    pageStack->addWidget(randomMapPage);

    // 设置默认显示的页面
    this->pageStack->setCurrentWidget(randomMapPage);

    // 布局：将 QStackedWidget 放入主窗口
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageStack);
}

WindowManager::~WindowManager() {
    delete startButton;
    delete settingsButton;
    delete aboutButton;
    delete exitButton;
}
