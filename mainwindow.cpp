#include "mainwindow.h"

MainWindow::MainWindow(Widget *parent) : Widget(parent), pageStack(new QStackedWidget(this))
{
    // 设置窗口标志，禁止拖拽
    // setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    // 禁用标题栏关闭按钮
    // setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    // 设置应用图标
    QIcon appIcon(":/image/taiji.png");
    this->setWindowTitle("提笔乾坤");
    this->setWindowIcon(appIcon);

    dataManager = new DataManager(this);
    socketManager = new SocketManager(this);
    // dataManager->clearAllRankings();

    // 创建 mainPage，并将四个按钮放入
    mainPage = new QWidget(this);

    // 创建标题标签
    QLabel *titleLabel = new QLabel("提笔乾坤", mainPage);
    QFont titleFont;
    titleFont.setFamily("华文行楷"); // 设置字体为行楷
    titleFont.setPointSize(100);     // 增大字体大小
    titleFont.setBold(true);         // 设置为粗体
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);    // 居中对齐
    titleLabel->setContentsMargins(0, 30, 0, 30); // 设置上下边距
    titleLabel->setStyleSheet("color: rgb(54,52,72);");

    startButton = new Lbutton(mainPage, "🚀 开始游戏", "black");
    aboutButton = new Lbutton(mainPage, "📖 关于", "black");
    settingButton = new Lbutton(mainPage, "⚙️ 设置", "black");
    exitButton = new Lbutton(mainPage, "👋 退出", "black");

    // 退出按钮连接到窗口关闭
    QObject::connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 定义按钮和布局边距的局部变量
    int buttonHorizontalSpacing = 200; // 按钮水平间距
    int buttonVerticalSpacing = 100;   // 按钮垂直间距

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
    page1Layout->addStretch(2);         // 上方弹性空间增加
    page1Layout->addWidget(titleLabel); // 添加标题
    page1Layout->addSpacing(100);       // 标题和按钮之间添加固定间距
    page1Layout->addWidget(buttonWidget, 0, Qt::AlignCenter);
    page1Layout->addStretch(3); // 下方弹性空间更大，使整体偏上
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
    randomMapMsgBox->hide();
    customMapMsgBox = new MapMsgBox(this);
    customMapMsgBox->hide();
    timeLimitMsgBox = new TimeLimitMsgBox(this);
    timeLimitMsgBox->hide();
    saveRandomMapMsgBox = new SaveMapMsgBox(this);
    saveRandomMapMsgBox->hide();
    saveCustomMapMsgBox = new SaveMapMsgBox(this);
    saveCustomMapMsgBox->hide();
    gamePage = new Game(this, dataManager);
    rankPage = new RankPage(this, dataManager);
    settingPage = new Setting(this, dataManager);
    onlineModePage = new OnlineMode(this, socketManager);

    // 修改所有页面切换的连接，使用switchPage替代setCurrentWidget
    connect(startPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(mainPage); });

    connect(aboutPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(mainPage); });

    connect(settingPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(mainPage); });

    connect(startButton, &QPushButton::clicked, this, [this]()
            { switchPage(startPage); });

    connect(aboutButton, &QPushButton::clicked, this, [this]()
            { switchPage(aboutPage); });

    connect(settingButton, &QPushButton::clicked, this, [this]()
            { switchPage(settingPage); });

    connect(startPage->YESmessageBox->closeButton, &QPushButton::clicked, this, [this]()
            {
        startPage->YESmessageBox->accept();
        switchPage(menuPage); });

    connect(menuPage->logoutButton, &QPushButton::clicked, this, [this]()
            {
        startPage->currentUserId.clear();  // 清除当前用户ID
        switchPage(startPage); });

    connect(menuPage->levelEditorButton, &QPushButton::clicked, this, [this]()
            { switchPage(levelEditorPage); });

    connect(menuPage->levelModeButton, &QPushButton::clicked, this, [this]()
            { switchPage(levelModePage); });

    connect(menuPage->rankButton, &QPushButton::clicked, this, [this]()
            { switchPage(rankPage); });

    connect(menuPage->onlineButton, &QPushButton::clicked, this, [this]()
            { switchPage(onlineModePage); });

    connect(onlineModePage->cancelButton, &QPushButton::clicked, this, [this]()
            {
        socketManager->closeConnection();  // 关闭连接
        if (gamePage->getOnlineMode())
            {
                gamePage->setOnlineMode(false, nullptr);  // 重置游戏页面的联机模式
            }
        switchPage(menuPage); });

    connect(onlineModePage->msgBox, &OnlineMsgBox::enterLevelMode, this, [this](bool isServer)
            {
        switchPage(levelModePage);
          gamePage->setOnlineMode(isServer, socketManager); });

    connect(onlineModePage->msgBox, &OnlineMsgBox::clientConnected, this, [this]()
            { 
                //qDebug() << "客户端成功连接到服务器";
                gamePage->setOnlineMode(false, socketManager); });

    connect(levelEditorPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(menuPage); });

    connect(randomMapMsgBox, &MapMsgBox::sendMsg, this, [this](int rings, QColor color1, QColor color2, QColor color3)
            {
        randomMapPage->generateHexagons(rings, color1, color2, color3);
        randomMapMsgBox->hide();
        switchPage(randomMapPage); });

    connect(customMapMsgBox, &MapMsgBox::sendMsg, this, [this](int rings, QColor color1, QColor color2, QColor color3)
            {
        customMapPage->generateHexagons(rings, color1, color2, color3);
        customMapMsgBox->hide();
        switchPage(customMapPage); });

    connect(socketManager, &SocketManager::gameStateReceived, this, &MainWindow::onClientReceivedGameState);

    for (int i = 0; i < 12; i++)
    {
        connect(levelModePage->buttons[i], &QPushButton::clicked, this, [this, i]()
                {
                if (gamePage->getOnlineMode())
                    {
                        if (socketManager->isServerMode())   // 如果是服务器端
                            {
                                MapData mapData = dataManager->getMap(i + 1);
                                if(mapData.generation == "")
                                    {
                                        MessageBox* msgBox = new MessageBox(this);
                                        msgBox->setMessage("该关卡未生成，请先保存关卡");
                                        connect(msgBox->closeButton, &QPushButton::clicked, this, [msgBox]()
                                        {
                                            msgBox->accept();
                                        });
                                        msgBox->exec();
                                        return;
                                    }
                                gamePage->setOnlineMode(true, socketManager);
                                socketManager->SendGameState(mapData);
                                gamePage->setMap(mapData);
                                switchPage(gamePage);
                            }
                    }
                else
                    {
                        MapData mapData = dataManager->getMap(i + 1);
                        if(mapData.generation == "")
                            {
                                MessageBox* msgBox = new MessageBox(this);
                                msgBox->setMessage("该关卡未生成，请先保存关卡");
                                connect(msgBox->closeButton, &QPushButton::clicked, this, [msgBox]()
                                {
                                    msgBox->accept();
                                });
                                msgBox->exec();
                                return;
                            }
                        gamePage->setMap(mapData);
                        switchPage(gamePage);
                    } });
    }

    connect(levelEditorPage->randomButton, &QPushButton::clicked, this, [this]()
            { randomMapMsgBox->show(); });

    connect(levelEditorPage->customButton, &QPushButton::clicked, this, [this]()
            { customMapMsgBox->show(); });

    connect(randomMapPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(levelEditorPage); });

    connect(customMapPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(levelEditorPage); });

    connect(randomMapPage->saveButton, &QPushButton::clicked, this, [this]()
            { saveRandomMapMsgBox->show(); });

    connect(customMapPage->saveButton, &QPushButton::clicked, this, [this]()
            { timeLimitMsgBox->show(); });
    connect(timeLimitMsgBox->closeButton, &QPushButton::clicked, this, [this]()
            {
        timeLimitMsgBox->hide();
        int timeLimit = timeLimitMsgBox->getSpinBoxValue();

        Animation* animation = new Animation(this);
        animation->show();

        // 创建定时器用于检查时间限制
        QTimer* solveTimer = new QTimer(this);
        QElapsedTimer* elapsedTimer = new QElapsedTimer();
        elapsedTimer->start();

        // 连接定时器超时信号
        connect(solveTimer, &QTimer::timeout, this, [this, animation, solveTimer, timeLimit, elapsedTimer]()
        {
            // 检查是否超过时间限制
            if (elapsedTimer->elapsed() >= timeLimit * 1000)
                {
                    solveTimer->stop();
                    delete solveTimer;
                    delete elapsedTimer;
                    animation->close();
                    delete animation;
                    MessageBox* msgBox1 = new MessageBox(this);
                    msgBox1->setMessage("未找到解决方案");
                    connect(msgBox1->closeButton, &QPushButton::clicked, this, [msgBox1]()
                    {
                        msgBox1->accept();
                    });
                    msgBox1->exec();

                    MessageBox* msgBox2 = new MessageBox(this, true);
                    msgBox2->setMessage("确定保存关卡吗？\n保存关卡后没有可用提示");
                    connect(msgBox2->closeButton, &QPushButton::clicked, this, [this, msgBox2]()
                    {
                        msgBox2->accept();
                        saveCustomMapMsgBox->show();
                    });
                    connect(msgBox2->cancelButton, &QPushButton::clicked, this, [msgBox2]()
                    {
                        msgBox2->accept();
                    });
                     
                    msgBox2->exec();
                    return;
                }

            // 尝试求解
            int f = customMapPage->solvePuzzle();
            if (f == 1)
                {
                    solveTimer->stop();
                    delete solveTimer;
                    delete elapsedTimer;
                    animation->close();
                    delete animation;
                    MessageBox* msgBox = new MessageBox(this);
                    msgBox->setMessage("求解成功！");
                    connect(msgBox->closeButton, &QPushButton::clicked, this, [msgBox]()
                    {
                        msgBox->accept();
                    });
                    msgBox->exec();
                    saveCustomMapMsgBox->show();
                }
            else if (f == 2)
                {
                    qDebug() << "本次求解未找到解决方案";
                }
            else if (f == 0)
                {
                    solveTimer->stop();
                    delete solveTimer;
                    delete elapsedTimer;
                    animation->close();
                    delete animation;
                    MessageBox* msgBox = new MessageBox(this);
                    msgBox->setMessage("关卡无解");
                    connect(msgBox->closeButton, &QPushButton::clicked, this, [msgBox]()
                    {
                        msgBox->accept();
                    });
                    msgBox->exec();
                }
        });

        // 启动定时器，每500毫秒尝试一次求解
        solveTimer->start(500); });

    connect(saveRandomMapMsgBox, &SaveMapMsgBox::sendMsg, this, [this](int msg)
            {
        randomMapPage->setId(msg);
        dataManager->addMap(randomMapPage->getMapData());
        dataManager->saveToFile(); });

    connect(saveCustomMapMsgBox, &SaveMapMsgBox::sendMsg, this, [this](int msg)
            {
        customMapPage->setId(msg);
        dataManager->addMap(customMapPage->getMapData());
        dataManager->saveToFile(); });

    connect(levelModePage->backButton, &QPushButton::clicked, this, [this]()
            {
        if (gamePage->getOnlineMode())
            {
                // 显示退出房间确认对话框
                MessageBox* msgBox = new MessageBox(this);
                connect(msgBox->closeButton, &QPushButton::clicked, this, [msgBox]()
                {
                    msgBox->accept();
                });
                msgBox->setMessage("确定要退出房间吗？");
                int result = msgBox->exec();
                if (result == QDialog::Accepted)
                    {
                        // 用户确认退出房间
                        socketManager->closeConnection();  // 函数内部会发送退出消息
                        gamePage->setOnlineMode(false, nullptr);
                        switchPage(onlineModePage);
                    }
                // 如果用户取消，则不执行后续操作
                return;
            }
        else
            {
                switchPage(menuPage);
            } });

    connect(gamePage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(levelModePage); });

    connect(rankPage->backButton, &QPushButton::clicked, this, [this]()
            { switchPage(menuPage); });

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
                switchPage(levelModePage);
            } });

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
    this->pageStack->setCurrentWidget(mainPage);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageStack);
}

void MainWindow::onClientReceivedGameState(const MapData &mapData)
{
    if (socketManager && !socketManager->isServerMode()) // 确保是客户端
    {
        // gamePage->setOnlineMode(true, socketManager); // 配置gamePage为客户端模式
        gamePage->setMap(mapData); // 设置地图
        switchPage(gamePage);      // 切换到游戏页面
    }
}

void MainWindow::switchPage(QWidget *nextPage)
{

    if (currentPage)
    {
        fadeOutPage(currentPage, nextPage);
    }
    else
    {
        fadeInPage(nextPage);
    }
    currentPage = nextPage;
}

void MainWindow::fadeInPage(QWidget *page)
{
    // 创建不透明度效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(page);
    page->setGraphicsEffect(effect);

    // 创建动画
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(animationDuration);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // 显示页面并开始动画
    pageStack->setCurrentWidget(page);

    // 如果是切换到游戏页面，在动画完成后触发粒子效果
    if (page == gamePage)
    {
        connect(animation, &QPropertyAnimation::finished, this, [this]()
                { gamePage->triggerAllHexEffects(); });
    }

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::fadeOutPage(QWidget *currentPage, QWidget *nextPage)
{
    // 创建不透明度效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(currentPage);
    currentPage->setGraphicsEffect(effect);

    // 创建动画
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(animationDuration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // 连接动画完成信号，切换到下一个页面
    connect(animation, &QPropertyAnimation::finished, this, [this, nextPage]()
            { fadeInPage(nextPage); });

    // 开始动画
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

MainWindow::~MainWindow()
{
    delete startButton;
    delete settingButton;
    delete aboutButton;
    delete exitButton;
}
