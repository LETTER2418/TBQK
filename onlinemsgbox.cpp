#include "onlinemsgbox.h"
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

OnlineMsgBox::OnlineMsgBox(QWidget *parent, SocketManager *manager)
    : QWidget(parent), socketManager(manager), isConnected(false), backgroundGif(nullptr), usingGif(false), currentMode(CreateMode) // 默认创建模式
{

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setWindowModality(Qt::ApplicationModal);
    setFixedSize(300, 400);

    // 加载背景图片/视频
    // setBackgroundImage(":/image/msg.png");
    setBackgroundGif(":/image/msg.gif");

    // 创建UI组件
    ipLabel = new QLabel("服务器IP:", this);
    localIPLabel = new QLabel("本机IP:      " + getLocalIP(), this);
    portLabel = new QLabel("端口:", this);

    // 设置字体大小
    int fontSize = 12;
    QFont labelFont;
    labelFont.setPointSize(fontSize); // 增大文字大小
    ipLabel->setFont(labelFont);
    localIPLabel->setFont(labelFont);
    portLabel->setFont(labelFont);

    ipInput = new QLineEdit("127.0.0.1", this);
    portInput = new QLineEdit("8888", this);
    portInput->setMaximumWidth(100);

    // 为输入框设置相同的字体大小
    ipInput->setFont(labelFont);
    portInput->setFont(labelFont);

    // 创建按钮
    cancelButton = new Lbutton(this, "取消", "black");
    actionButton = new Lbutton(this, "创建", "black"); // 默认显示"创建"

    // 布局
    QGridLayout *topLayout = new QGridLayout();
    topLayout->setContentsMargins(10, 5, 10, 5); // 设置上边距较小，使标签更靠近顶部
    topLayout->setHorizontalSpacing(1);          // 设置水平间距为5像素

    topLayout->addWidget(localIPLabel, 0, 0, 1, 2);
    topLayout->setRowMinimumHeight(1, 15); // 添加15像素的垂直间距
    topLayout->addWidget(portLabel, 4, 0);
    topLayout->addWidget(portInput, 4, 1);
    topLayout->setRowMinimumHeight(3, 15); // 添加15像素的垂直间距
    topLayout->addWidget(ipLabel, 2, 0);
    topLayout->addWidget(ipInput, 2, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(actionButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch(1); // 添加一个弹性空间，将内容推向顶部
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    connect(socketManager, &SocketManager::connectionError, this, &OnlineMsgBox::handleConnectionError);
    connect(socketManager, &SocketManager::clientConnected, this, &OnlineMsgBox::handleClientConnected); // 本地客户端成功连接到远程服务器
    connect(socketManager, &SocketManager::clientDisconnected, this, &OnlineMsgBox::handleClientDisconnected);
    connect(socketManager, &SocketManager::navigateToPageRequest, this, &OnlineMsgBox::handleNavigateRequest);
    connect(socketManager, &SocketManager::newClientConnected, this, &OnlineMsgBox::handleNewClientConnected); // 远程客户端连接到本地服务器
    connect(socketManager, &SocketManager::roomLeft, this, &OnlineMsgBox::handleRoomLeft);                     // 对方退出房间
    connect(socketManager, &SocketManager::duplicateIdDetected, this, &OnlineMsgBox::handleDuplicateId);       // 处理ID重复

    // 连接取消和创建/加入按钮的信号
    connect(cancelButton, &QPushButton::clicked, this, &QWidget::hide);
    connect(actionButton, &QPushButton::clicked, this, [this]()
            {
        if (currentMode == CreateMode)
            {
                // 创建房间模式：启动服务器
                if(startServer())
                    {
                        hide();
                        emit enterLevelMode();
                    }
            }
        else
            {
                // 加入房间模式：连接到服务器
                connectToServer();
            } });

    msgBox = new MessageBox(this);
    connect(msgBox->closeButton, &QPushButton::clicked, this, [this]()
            { msgBox->accept(); });
}

void OnlineMsgBox::setBackgroundImage(const QString &path)
{
    if (backgroundGif)
    {
        backgroundGif->stop();
        delete backgroundGif;
        backgroundGif = nullptr;
    }

    usingGif = false;
    backgroundImage = QPixmap(path);
    update(); // 触发重绘
}

void OnlineMsgBox::setBackgroundGif(const QString &path)
{
    if (backgroundGif)
    {
        backgroundGif->stop();
        delete backgroundGif;
    }

    backgroundGif = new QMovie(path);
    if (backgroundGif->isValid())
    {
        usingGif = true;
        connect(backgroundGif, &QMovie::frameChanged, this, &OnlineMsgBox::updateBackground);
        backgroundGif->start();
    }
    else
    {
        // GIF无效，回退到静态图像
        delete backgroundGif;
        backgroundGif = nullptr;
        usingGif = false;
        backgroundImage = QPixmap(":/image/msg.png");
        update();
    }
}

void OnlineMsgBox::updateBackground()
{
    if (backgroundGif && usingGif)
    {
        currentFrame = QPixmap::fromImage(backgroundGif->currentImage());
        update(); // 触发重绘
    }
}

void OnlineMsgBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 根据是否使用GIF绘制不同的背景
    if (usingGif && !currentFrame.isNull())
    {
        // 绘制GIF当前帧
        painter.drawPixmap(rect(), currentFrame, currentFrame.rect());
    }
    else
    {
        // 绘制静态背景图片
        painter.drawPixmap(rect(), backgroundImage, backgroundImage.rect());
    }

    // 调用基类的paintEvent以确保正常绘制其他部件
    QWidget::paintEvent(event);
}

OnlineMsgBox::~OnlineMsgBox()
{
    if (backgroundGif)
    {
        backgroundGif->stop();
        delete backgroundGif;
    }
}

QString OnlineMsgBox::getLocalIP()
{
    QString localIP = "未知";

    // 遍历所有网络接口
    foreach (QNetworkInterface networkInterface, QNetworkInterface::allInterfaces())
    {
        // 检查接口是否活跃且不是回环接口
        if (networkInterface.flags().testFlag(QNetworkInterface::IsUp) &&
            !networkInterface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {

            QString interfaceName = networkInterface.name();
            QString humanReadableName = networkInterface.humanReadableName();

            // 扩展检测条件，包括更多可能的WLAN标识
            bool isWlan = interfaceName.toLower().contains("wlan") ||
                          humanReadableName.toLower().contains("wlan") ||
                          humanReadableName.toLower().contains("wi-fi") ||
                          humanReadableName.toLower().contains("wi fi") ||
                          humanReadableName.toLower().contains("wireless") ||
                          humanReadableName.toLower().contains("无线");

            // 如果接口名称或描述包含WLAN相关字样，或者是特定的网络接口类型
            if (isWlan)
            {
                // 获取此适配器的IPv4地址
                foreach (QNetworkAddressEntry entry, networkInterface.addressEntries())
                {
                    QHostAddress ip = entry.ip();
                    if (ip.protocol() == QAbstractSocket::IPv4Protocol && !ip.isLoopback())
                    {
                        localIP = ip.toString();
                        return localIP;
                    }
                }
            }
        }
    }

    // 如果没找到WLAN适配器，则回退到获取任何可用的非本地IPv4地址
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address : QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
        {
            localIP = address.toString();
            qDebug() << "使用备用IPv4地址:" << localIP;
            break;
        }
    }

    return localIP;
}

bool OnlineMsgBox::startServer()
{
    bool ok = false;
    int port = portInput->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535)
    {
        // 使用自定义MessageBox替换QMessageBox
        msgBox->setMessage("请输入有效的端口号(1-65535)");
        msgBox->exec();
        return false;
    }

    // 假设SocketManager已实现StartServer方法
    if (socketManager->StartServer())
    {
        hide();
        isConnected = true;
        // 创建房间成功提示
        msgBox->setMessage("房间创建成功\n等待对方加入\n\n房间IP: " + getLocalIP() + "\n端口: " + portInput->text());
        msgBox->exec();
        return true;
    }
    else
    {
        // 创建失败提示
        msgBox->setMessage("创建房间失败，端口" + portInput->text() + "可能已被占用");
        msgBox->exec();
        return false;
    }
}

bool OnlineMsgBox::connectToServer()
{
    QString ip = ipInput->text();
    bool ok = false;
    int port = portInput->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535)
    {
        msgBox->setMessage("请输入有效的端口号(1-65535)");
        msgBox->exec();
        return false;
    }
    hide();
    socketManager->StartClient(ip);
    return true;
}

void OnlineMsgBox::handleConnectionError(const QString &error)
{
    isConnected = false;
    msgBox->setMessage("connection error: " + error);
    msgBox->exec();
}

void OnlineMsgBox::handleClientConnected()
{
    isConnected = true;

    msgBox->setMessage("成功连接到服务器！");
    msgBox->exec();
}

void OnlineMsgBox::handleClientDisconnected()
{
    isConnected = false;
}

void OnlineMsgBox::setMode(Mode mode)
{
    currentMode = mode;
    if (mode == CreateMode)
    {
        actionButton->setText("创建");
    }
    else
    {
        actionButton->setText("加入");
    }
}

void OnlineMsgBox::changeIPLabelInputVisibility(bool visible)
{
    ipLabel->setVisible(visible);
    ipInput->setVisible(visible);
}

void OnlineMsgBox::handleNavigateRequest(const QString &pageName)
{
    if (pageName == "levelModePage")
    {
        // 客户端收到指令，切换到levelModePage
        // 它通过发出 enterLevelMode 信号，让 MainWindow 处理页面切换
        emit enterLevelMode();
        hide();
    }
}

// 添加showEvent函数实现居中显示
void OnlineMsgBox::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 如果有父控件，则在父控件中居中显示
    if (parentWidget())
    {
        QPoint parentCenter = parentWidget()->mapToGlobal(parentWidget()->rect().center());
        QRect childRect = rect();
        int x = parentCenter.x() - childRect.width() / 2;
        int y = parentCenter.y() - childRect.height() / 2;
        move(x, y);
    }

    localIPLabel->setText("本机IP:      " + getLocalIP());
}

// 添加处理新客户端连接的方法
void OnlineMsgBox::handleNewClientConnected()
{
    msgBox->setMessage("有新玩家加入房间！");
    msgBox->exec();
}

// 添加处理对方退出房间的方法
void OnlineMsgBox::handleRoomLeft()
{
    // 显示对方退出房间的提示
    msgBox->setMessage("对方已退出房间！");
    msgBox->exec();
}

// 处理ID重复的情况
void OnlineMsgBox::handleDuplicateId()
{
    // 服务端检测到客户端ID重复，始终显示警告消息
    msgBox->setMessage("检测到重复ID的客户端尝试加入，已拒绝连接");
    msgBox->exec();

    // 确保消息框不会阻塞其他操作
    QCoreApplication::processEvents();
}
