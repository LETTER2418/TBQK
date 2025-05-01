#include "mainwindow.h"

MainWindow::MainWindow(Widget *parent) : Widget(parent), pageStack(new QStackedWidget(this))
{

    mapManager = new MapManager(this);

    // 创建 mainPage，并将四个按钮放入
    mainPage = new QWidget(this);

    startButton = new Lbutton(mainPage, "开始游戏");
    aboutButton = new Lbutton(mainPage, "关于");
    settingsButton = new Lbutton(mainPage, "设置");
    exitButton = new Lbutton(mainPage, "退出");

    // 退出按钮连接到窗口关闭
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(startButton, 0, 0);
    layout->addWidget(aboutButton, 1, 0);
    layout->addWidget(settingsButton, 0, 1);
    layout->addWidget(exitButton, 1, 1);

    // 创建一个 QWidget 作为中央部件
    QWidget *buttonWidget = new QWidget(mainPage);
    buttonWidget->setLayout(layout);
    buttonWidget->setFixedSize(400, 200);  // 固定大小

    // 创建一个布局让 buttonWidget 居中
    QVBoxLayout *page1Layout = new QVBoxLayout(mainPage);
    page1Layout->addStretch();            // 上方弹性空间
    page1Layout->addWidget(buttonWidget, 0, Qt::AlignCenter);
    page1Layout->addStretch();            // 下方弹性空间
    mainPage->setLayout(page1Layout);


    // 初始化各页面
    startPage = new Start(this);
    aboutPage = new About(this);
    menuPage = new Menu(this);
    levelEditorPage = new LevelEditor(this);
    randomMapPage = new RandomMap(this);
    randomMapMsgBox = new RandomMapMsgBox(this);
    randomMapMsgBox->close();
    saveMapMsgBox= new SaveMapMsgBox(this);
    saveMapMsgBox->close();

    // 连接信号与槽
    connect(startPage->backButton, &QPushButton::clicked, this, [this]() {
        pageStack->setCurrentWidget(mainPage);
    });

    connect(aboutPage->backButton, &QPushButton::clicked, this, [this]() {
        pageStack->setCurrentWidget(mainPage);
    });

    connect(startButton, &QPushButton::clicked, this, [this]() {
        pageStack->setCurrentWidget(startPage);
    });

    connect(aboutButton, &QPushButton::clicked, this, [this]() {
        pageStack->setCurrentWidget(aboutPage);
    });

    connect(startPage->YESmessageBox->closeButton, &QPushButton::clicked, this, [this]() {
        startPage->YESmessageBox->accept();
        pageStack->setCurrentWidget(menuPage);
    });

    connect(menuPage->logoutButton, &QPushButton::clicked, this,  [this](){
        pageStack->setCurrentWidget(startPage);
    });

    connect(menuPage->levelEditorButton, &QPushButton::clicked, this, [this](){
        pageStack->setCurrentWidget(levelEditorPage);
    });

    connect(levelEditorPage->backButton,&QPushButton::clicked, this, [this](){
        pageStack->setCurrentWidget(menuPage);
    });

    connect(randomMapMsgBox->closeButton,&QPushButton::clicked, this, [this](){
        auto t = randomMapMsgBox;
        randomMapPage->generateHexagons(t->rings, t->color1, t->color2);
        randomMapMsgBox->close();
        pageStack->setCurrentWidget(randomMapPage);
    });

    connect(levelEditorPage->randomButton,&QPushButton::clicked, this, [this](){
        randomMapMsgBox->show();
    });

    connect(randomMapPage->backButton,&QPushButton::clicked, this, [this](){
        pageStack->setCurrentWidget(levelEditorPage);
    });

    connect(randomMapPage->saveButton, &QPushButton::clicked, this, [this](){
        saveMapMsgBox->show();
    });

    connect(saveMapMsgBox, &SaveMapMsgBox::sendMsg, this, [this](int msg){
        randomMapPage->setId(msg);
        mapManager->addMap(randomMapPage->getMapData());
        mapManager->saveMap();
    });

    // 将页面添加到 QStackedWidget
    pageStack->addWidget(mainPage);
    pageStack->addWidget(startPage);
    pageStack->addWidget(aboutPage);
    pageStack->addWidget(menuPage);
    pageStack->addWidget(levelEditorPage);
    pageStack->addWidget(randomMapPage);

    // 设置默认显示的页面
    this->pageStack->setCurrentWidget(levelEditorPage);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageStack);



}

MainWindow::~MainWindow() {
    delete startButton;
    delete settingsButton;
    delete aboutButton;
    delete exitButton;
}
