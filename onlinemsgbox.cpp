#include "onlinemsgbox.h"
#include <QDateTime>
#include <QDebug>

OnlineMsgBox::OnlineMsgBox(QWidget *parent, SocketManager* manager)
    : QWidget(parent)
    , socketManager(manager)
    , isConnected(false)
    , backgroundGif(nullptr)
    , usingGif(false)
    , currentMode(CreateMode)  // 默认创建模式
{
    setWindowTitle("在线聊天");
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setGeometry(700, 250, 300, 400);

    // 加载背景图片/视频
    //setBackgroundImage(":/image/msg.png");
    setBackgroundGif(":/image/msg.gif");
    
    // 创建UI组件
    ipLabel = new QLabel("服务器IP:", this);
    localIPLabel = new QLabel("本机IP: " + getLocalIP(), this);
    portLabel = new QLabel("端口:", this);
    
    ipInput = new QLineEdit("127.0.0.1", this);
    portInput = new QLineEdit("8888", this);
    portInput->setMaximumWidth(100);
    
    // 创建按钮
    cancelButton = new Lbutton(this, "取消", "black");
    actionButton = new Lbutton(this, "创建", "black");  // 默认显示"创建"

    // 布局
    QGridLayout *topLayout = new QGridLayout();
    topLayout->addWidget(localIPLabel, 0, 0, 1, 2);
    topLayout->addWidget(ipLabel, 1, 0);
    topLayout->addWidget(ipInput, 1, 1);
    topLayout->addWidget(portLabel, 2, 0);
    topLayout->addWidget(portInput, 2, 1);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(actionButton);
    buttonLayout->addStretch();
    
    
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
    
    // 设置窗口尺寸
    resize(300, 400);
    
    connect(socketManager, &SocketManager::connectionError, this, &OnlineMsgBox::handleConnectionError);
    connect(socketManager, &SocketManager::clientConnected, this, &OnlineMsgBox::handleClientConnected);
    connect(socketManager, &SocketManager::clientDisconnected, this, &OnlineMsgBox::handleClientDisconnected);
    connect(socketManager, &SocketManager::navigateToPageRequest, this, &OnlineMsgBox::handleNavigateRequest);
    
    // 连接取消和创建/加入按钮的信号
    connect(cancelButton, &QPushButton::clicked, this, &QWidget::hide);
    connect(actionButton, &QPushButton::clicked, this, [this]() {
        if (currentMode == CreateMode) {
            // 创建房间模式：进入levelModePage，启动服务器
            emit enterLevelMode(); // 主机端直接切换
            hide();
            startServer(); // 服务器启动后，handleNewConnection会给连入的客户端发消息
        } else {
            // 加入房间模式：连接到服务器
            connectToServer(); // 客户端连接，会设置socketManager并监听导航消息
        }
    });
}

void OnlineMsgBox::setBackgroundImage(const QString& path)
{
    if (backgroundGif) {
        backgroundGif->stop();
        delete backgroundGif;
        backgroundGif = nullptr;
    }
    
    usingGif = false;
    backgroundImage = QPixmap(path);
    update(); // 触发重绘
}

void OnlineMsgBox::setBackgroundGif(const QString& path)
{
    if (backgroundGif) {
        backgroundGif->stop();
        delete backgroundGif;
    }
    
    backgroundGif = new QMovie(path);
    if (backgroundGif->isValid()) {
        usingGif = true;
        connect(backgroundGif, &QMovie::frameChanged, this, &OnlineMsgBox::updateBackground);
        backgroundGif->start();
    } else {
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
    if (backgroundGif && usingGif) {
        currentFrame = QPixmap::fromImage(backgroundGif->currentImage());
        update(); // 触发重绘
    }
}

void OnlineMsgBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    
    // 根据是否使用GIF绘制不同的背景
    if (usingGif && !currentFrame.isNull()) {
        // 绘制GIF当前帧
        painter.drawPixmap(rect(), currentFrame, currentFrame.rect());
    } else {
        // 绘制静态背景图片
        painter.drawPixmap(rect(), backgroundImage, backgroundImage.rect());
    }
    
    // 调用基类的paintEvent以确保正常绘制其他部件
    QWidget::paintEvent(event);
}

OnlineMsgBox::~OnlineMsgBox()
{
    if (backgroundGif) {
        backgroundGif->stop();
        delete backgroundGif;
    }
}

QString OnlineMsgBox::getLocalIP()
{
    QString localIP = "未知";
    
    //遍历所有网络接口
    foreach(QNetworkInterface networkInterface, QNetworkInterface::allInterfaces()) {
        // 检查接口是否活跃且不是回环接口
        if (networkInterface.flags().testFlag(QNetworkInterface::IsUp) && 
            !networkInterface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
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
            if (isWlan) {
                // 获取此适配器的IPv4地址
                foreach(QNetworkAddressEntry entry, networkInterface.addressEntries()) {
                    QHostAddress ip = entry.ip();
                    if (ip.protocol() == QAbstractSocket::IPv4Protocol && !ip.isLoopback()) {
                        localIP = ip.toString();
                        return localIP;
                    }
                }
            }
        }
    }
    
    // 如果没找到WLAN适配器，则回退到获取任何可用的非本地IPv4地址
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
            localIP = address.toString();
            qDebug() << "使用备用IPv4地址:" << localIP;
            break;
        }
    }
    
    return localIP;
}

void OnlineMsgBox::startServer()
{
    bool ok = false;
    int port = portInput->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        QMessageBox::warning(this, "错误", "请输入有效的端口号(1-65535)");
        return;
    }
    
    // 假设SocketManager已实现StartServer方法
    if (socketManager->StartServer()) {
        isConnected = true;
    } else {
        qDebug() << "服务器启动失败，端口:" << portInput->text();
    }
}

void OnlineMsgBox::connectToServer()
{
    
    QString ip = ipInput->text();
    bool ok = false;
    int port = portInput->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        QMessageBox::warning(this, "错误", "请输入有效的端口号(1-65535)");
        return;
    }

    socketManager->StartClient(ip);
}


void OnlineMsgBox::handleConnectionError(const QString& error)
{
    qDebug() << "连接错误:" << error;
    isConnected = false;
}

void OnlineMsgBox::handleClientConnected()
{
    isConnected = true;
}

void OnlineMsgBox::handleClientDisconnected()
{
    isConnected = false;
}

void OnlineMsgBox::setMode(Mode mode)
{
    currentMode = mode;
    if (mode == CreateMode) {
        actionButton->setText("创建");
    } else {
        actionButton->setText("加入");
    }
}

void OnlineMsgBox::handleNavigateRequest(const QString& pageName)
{
    if (pageName == "levelModePage") {
        // 客户端收到指令，切换到levelModePage
        // 它通过发出 enterLevelMode 信号，让 MainWindow 处理页面切换
        emit enterLevelMode();
        hide();
    }
}
