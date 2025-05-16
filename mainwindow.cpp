#include "mainwindow.h"

MainWindow::MainWindow(Widget *parent) : Widget(parent), pageStack(new QStackedWidget(this))
{
    // 设置窗口标志，禁止拖拽
    //setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    // 禁用标题栏关闭按钮
    //setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    // 设置应用图标
    QIcon appIcon(":/image/taiji.png");
    this->setWindowIcon(appIcon);

    dataManager = new DataManager(this);
    socketManager = new SocketManager(this);
    //dataManager->clearAllRankings();

    // 创建 mainPage，并将四个按钮放入
    mainPage = new QWidget(this);

    startButton = new Lbutton(mainPage, "开始游戏");
    aboutButton = new Lbutton(mainPage, "关于");
    settingButton = new Lbutton(mainPage, "设置");
    exitButton = new Lbutton(mainPage, "退出");

    // 退出按钮连接到窗口关闭
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 定义按钮和布局边距的局部变量
    int buttonHorizontalSpacing = 200; // 按钮水平间距
    int buttonVerticalSpacing = 100;    // 按钮垂直间距

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(startButton, 0, 0);
    layout->addWidget(aboutButton, 1, 0);
    layout->addWidget(settingButton, 0, 1);
    layout->addWidget(exitButton, 1, 1);

    // 设置按钮之间的水平和垂直间距
    layout->setHorizontalSpacing(buttonHorizontalSpacing);
    layout->setVerticalSpacing(buttonVerticalSpacing);


    // 创建一个 QWidget 作为中央部件
    QWidget *buttonWidget = new QWidget(mainPage);
    buttonWidget->setLayout(layout);
    // buttonWidget->setFixedSize(400, 200);  // 固定大小

    // 创建一个布局让 buttonWidget 居中
    QVBoxLayout *page1Layout = new QVBoxLayout(mainPage);
    page1Layout->addStretch();            // 上方弹性空间
    page1Layout->addWidget(buttonWidget, 0, Qt::AlignCenter);
    page1Layout->addStretch();            // 下方弹性空间
    mainPage->setLayout(page1Layout);


    // 初始化各页面
    startPage = new Start(this, dataManager, socketManager);
    aboutPage = new About(this);
    menuPage = new Menu(this);
    levelEditorPage = new LevelEditor(this);
    randomMapPage = new RandomMap(this);
    customMapPage = new CustomMap(this);
    levelModePage = new LevelMode(this);
    randomMapMsgBox = new MapMsgBox(this);
    customMapMsgBox = new MapMsgBox(this);
    randomMapMsgBox->hide();
    saveRandomMapMsgBox = new SaveMapMsgBox(this);
    saveRandomMapMsgBox->hide();
    saveCustomMapMsgBox = new SaveMapMsgBox(this);
    saveCustomMapMsgBox->hide();
    gamePage = new Game(this, dataManager);
    rankPage = new RankPage(this, dataManager);
    settingPage = new Setting(this, dataManager);
    onlineModePage = new OnlineMode(this, socketManager);

    // 连接信号与槽
    connect(startPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(mainPage);
    });

    connect(aboutPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(mainPage);
    });

    connect(settingPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(mainPage);
    });

    connect(startButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(startPage);
    });

    connect(aboutButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(aboutPage);
    });

    connect(settingButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(settingPage);
    });

    connect(startPage->YESmessageBox->closeButton, &QPushButton::clicked, this, [this]()
    {
        startPage->YESmessageBox->accept();
        pageStack->setCurrentWidget(menuPage);
    });

    connect(menuPage->logoutButton, &QPushButton::clicked, this,  [this]()
    {
        startPage->currentUserId.clear();  // 清除当前用户ID
        pageStack->setCurrentWidget(startPage);
    });

    connect(menuPage->levelEditorButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(levelEditorPage);
    });

    connect(menuPage->levelModeButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(levelModePage);
    });

    connect(menuPage->rankButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(rankPage);
    });

    connect(menuPage->onlineButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(onlineModePage);
    });

    connect(onlineModePage->cancelButton, &QPushButton::clicked, this, [this]()
    {
        socketManager->closeConnection();  // 关闭连接
        if (gamePage->getOnlineMode())
            {
                gamePage->setOnlineMode(false, nullptr);  // 重置游戏页面的联机模式
            }
        pageStack->setCurrentWidget(menuPage);
    });

    connect(onlineModePage->msgBox, &OnlineMsgBox::enterLevelMode, this, [this]()
    {
        pageStack->setCurrentWidget(levelModePage);
        gamePage->setOnlineMode(true, socketManager);
    });

    connect(levelEditorPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(menuPage);
    });

    connect(randomMapMsgBox, &MapMsgBox::sendMsg, this, [this](int rings, QColor color1, QColor color2, QColor color3)
    {
        randomMapPage->generateHexagons(rings, color1, color2, color3);
        randomMapMsgBox->hide();
        pageStack->setCurrentWidget(randomMapPage);
    });

    connect(customMapMsgBox, &MapMsgBox::sendMsg, this, [this](int rings, QColor color1, QColor color2, QColor color3)
    {
        customMapPage->generateHexagons(rings, color1, color2, color3);
        customMapMsgBox->hide();
        pageStack->setCurrentWidget(customMapPage);
    });

    connect(socketManager, &SocketManager::gameStateReceived, this, &MainWindow::onClientReceivedGameState);

    for(int i = 0; i < 12; i++)
        {
            connect(levelModePage->buttons[i], &QPushButton::clicked, this, [this, i]()
            {
                if (gamePage->getOnlineMode())
                    {
                        if (socketManager->isServerMode())   // 如果是服务器端
                            {
                                MapData mapData = dataManager->getMap(i + 1);
                                gamePage->setOnlineMode(true, socketManager);
                                socketManager->SendGameState(mapData);
                                gamePage->setMap(mapData);
                                pageStack->setCurrentWidget(gamePage);
                            }
                    }
                else
                    {
                        MapData mapData = dataManager->getMap(i + 1);
                        gamePage->setMap(mapData);
                        pageStack->setCurrentWidget(gamePage);
                    }
            });
        }


    connect(levelEditorPage->randomButton, &QPushButton::clicked, this, [this]()
    {
        randomMapMsgBox->show();
    });

    connect(levelEditorPage->customButton, &QPushButton::clicked, this, [this]()
    {
        customMapMsgBox->show();
    });

    connect(randomMapPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(levelEditorPage);
    });

    connect(customMapPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(levelEditorPage);
    });

    connect(randomMapPage->saveButton, &QPushButton::clicked, this, [this]()
    {
        saveRandomMapMsgBox->show();
    });

    connect(customMapPage->saveButton, &QPushButton::clicked, this, [this]()
    {
        customMapPage->solvePuzzle();
        saveCustomMapMsgBox->show();
    });


    connect(saveRandomMapMsgBox, &SaveMapMsgBox::sendMsg, this, [this](int msg)
    {
        randomMapPage->setId(msg);
        dataManager->addMap(randomMapPage->getMapData());
        dataManager->saveToFile();
    });

    connect(saveCustomMapMsgBox, &SaveMapMsgBox::sendMsg, this, [this](int msg)
    {
        customMapPage->setId(msg);
        dataManager->addMap(customMapPage->getMapData());
        dataManager->saveToFile();
    });



    connect(levelModePage->backButton, &QPushButton::clicked, this, [this]()
    {
        if (gamePage->getOnlineMode())
            {
                socketManager->closeConnection();
                gamePage->setOnlineMode(false, nullptr);
                pageStack->setCurrentWidget(onlineModePage);
            }
        else
        {
            pageStack->setCurrentWidget(menuPage);
        }
    });

    connect(gamePage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(levelModePage);
    });

    connect(rankPage->backButton, &QPushButton::clicked, this, [this]()
    {
        pageStack->setCurrentWidget(menuPage);
    });

    // 连接游戏完成时返回关卡模式的信号
    connect(gamePage, &Game::returnToLevelMode, this, [this](bool completed, int penaltySeconds, int steps, int levelId)
    {
        // 只有当游戏完成并且有有效用户ID时才更新排行榜
        if (completed)
        {
            // 更新排行榜
            if(!startPage->currentUserId.isEmpty())
            {
                dataManager->updateRanking(levelId, startPage->currentUserId, penaltySeconds, steps);
            }
            pageStack->setCurrentWidget(levelModePage);
        }
    });

    // 将页面添加到 QStackedWidget
    pageStack->addWidget(mainPage);
    pageStack->addWidget(startPage);
    pageStack->addWidget(aboutPage);
    pageStack->addWidget(menuPage);
    pageStack->addWidget(levelEditorPage);
    pageStack->addWidget(randomMapPage);
    pageStack->addWidget(levelModePage);
    pageStack->addWidget(customMapPage);
    pageStack->addWidget(gamePage);
    pageStack->addWidget(rankPage);
    pageStack->addWidget(settingPage);
    pageStack->addWidget(onlineModePage);

    // 设置默认显示的页面
    this->pageStack->setCurrentWidget(startPage);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageStack);

}

void MainWindow::onClientReceivedGameState(const MapData& mapData)
{
    if (socketManager && !socketManager->isServerMode())   // 确保是客户端
        {
            gamePage->setOnlineMode(true, socketManager); // 配置gamePage为客户端模式
            gamePage->setMap(mapData);                           // 设置地图
            pageStack->setCurrentWidget(gamePage);             // 切换到游戏页面
        }
}

MainWindow::~MainWindow()
{
    delete startButton;
    delete settingButton;
    delete aboutButton;
    delete exitButton;
}
