#include "onlinemsgbox.h"
#include <QDateTime>

OnlineMsgBox::OnlineMsgBox(QWidget *parent)
    : QWidget{parent}
    , socketManager(nullptr)
    , isConnected(false)
    , backgroundGif(nullptr)
    , usingGif(false)
{
    setWindowTitle("在线聊天");
    
    // 加载背景图片
    //setBackgroundImage(":/image/msg.png");
    setBackgroundGif(":/image/msg.gif");
    
    // 创建UI组件
    ipLabel = new QLabel("服务器IP:", this);
    localIPLabel = new QLabel("本机IP: " + getLocalIP(), this);
    portLabel = new QLabel("端口:", this);
    
    ipInput = new QLineEdit("127.0.0.1", this);
    portInput = new QLineEdit("8888", this);
    portInput->setMaximumWidth(100);
    
    serverButton = new QPushButton("启动服务器", this);
    clientButton = new QPushButton("连接到服务器", this);
    
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);
    
    messageInput = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);
    sendButton->setEnabled(false);
    
    // 布局
    QGridLayout *topLayout = new QGridLayout();
    topLayout->addWidget(localIPLabel, 0, 0, 1, 2);
    topLayout->addWidget(ipLabel, 1, 0);
    topLayout->addWidget(ipInput, 1, 1);
    topLayout->addWidget(portLabel, 2, 0);
    topLayout->addWidget(portInput, 2, 1);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(serverButton);
    buttonLayout->addWidget(clientButton);
    
    QHBoxLayout *messageLayout = new QHBoxLayout();
    messageLayout->addWidget(messageInput);
    messageLayout->addWidget(sendButton);
    
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(chatDisplay);
    mainLayout->addLayout(messageLayout);
    
    setLayout(mainLayout);
    
    // 设置窗口尺寸
    resize(300, 400);
    
    // 连接信号
    connect(serverButton, &QPushButton::clicked, this, &OnlineMsgBox::startServer);
    connect(clientButton, &QPushButton::clicked, this, &OnlineMsgBox::connectToServer);
    connect(sendButton, &QPushButton::clicked, this, &OnlineMsgBox::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &OnlineMsgBox::sendMessage);//回车发送消息
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
    delete socketManager;
    
    if (backgroundGif) {
        backgroundGif->stop();
        delete backgroundGif;
    }
}

QString OnlineMsgBox::getLocalIP()
{
    QString localIP = "未知";
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
            localIP = address.toString();
            break;
        }
    }
    return localIP;
}

void OnlineMsgBox::startServer()
{
    if (socketManager) {
        delete socketManager;
    }
    
    socketManager = new SocketManager(this);
    
    // 连接SocketManager的信号
    connect(socketManager, &SocketManager::newMessageReceived, this, &OnlineMsgBox::displayMessage);
    connect(socketManager, &SocketManager::connectionError, this, &OnlineMsgBox::handleConnectionError);
    connect(socketManager, &SocketManager::clientConnected, this, &OnlineMsgBox::handleClientConnected);
    connect(socketManager, &SocketManager::clientDisconnected, this, &OnlineMsgBox::handleClientDisconnected);
    
    bool ok = false;
    int port = portInput->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        QMessageBox::warning(this, "错误", "请输入有效的端口号(1-65535)");
        return;
    }
    
    // 假设SocketManager已实现StartServer方法
    if (socketManager->StartServer()) {
        chatDisplay->append("<系统> 服务器已启动，等待连接...");
        serverButton->setEnabled(false);
        clientButton->setEnabled(false);
        sendButton->setEnabled(true);
        isConnected = true;
    } else {
        chatDisplay->append("<系统> 服务器启动失败");
    }
}

void OnlineMsgBox::connectToServer()
{
    if (socketManager) {
        delete socketManager;
    }
    
    socketManager = new SocketManager(this);
    
    // 连接SocketManager的信号
    connect(socketManager, &SocketManager::newMessageReceived, this, &OnlineMsgBox::displayMessage);
    connect(socketManager, &SocketManager::connectionError, this, &OnlineMsgBox::handleConnectionError);
    connect(socketManager, &SocketManager::clientConnected, this, &OnlineMsgBox::handleClientConnected);
    connect(socketManager, &SocketManager::clientDisconnected, this, &OnlineMsgBox::handleClientDisconnected);
    
    QString ip = ipInput->text();
    bool ok = false;
    int port = portInput->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        QMessageBox::warning(this, "错误", "请输入有效的端口号(1-65535)");
        return;
    }
    
    // 尝试连接，不再检查返回值
    socketManager->StartClient(ip);

    // 更新UI为"正在连接"状态
    chatDisplay->append("<系统> 正在连接到服务器...");
    serverButton->setEnabled(false);
    clientButton->setEnabled(false);
    sendButton->setEnabled(false); // 在连接成功前禁用发送按钮
    // isConnected 状态将在 handleClientConnected 中设置为 true
}

void OnlineMsgBox::sendMessage()
{
    if (!socketManager || !isConnected || messageInput->text().isEmpty()) {
        return;
    }
    
    QString message = messageInput->text();
    socketManager->SendChatMessage(message, "我");
    
    // 清空输入框
    messageInput->clear();
    messageInput->setFocus();
    
    // 在聊天框中显示自己发送的消息
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    chatDisplay->append(QString("%1 <我> %2").arg(timestamp).arg(message));
}

void OnlineMsgBox::displayMessage(const QString& sender, const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    chatDisplay->append(QString("%1 <%2> %3").arg(timestamp).arg(sender).arg(message));
}

void OnlineMsgBox::handleConnectionError(const QString& error)
{
    chatDisplay->append(QString("<系统> 错误: %1").arg(error));
    // 连接失败，恢复按钮状态
    serverButton->setEnabled(true);
    clientButton->setEnabled(true);
    sendButton->setEnabled(false);
    isConnected = false;
}

void OnlineMsgBox::handleClientConnected()
{
    chatDisplay->append("<系统> 已连接到服务器");
    serverButton->setEnabled(false); // 保持禁用状态
    clientButton->setEnabled(false); // 保持禁用状态
    sendButton->setEnabled(true);
    isConnected = true;
}

void OnlineMsgBox::handleClientDisconnected()
{
    chatDisplay->append("<系统> 客户端已断开连接");
    if (!socketManager) {
        serverButton->setEnabled(true);
        clientButton->setEnabled(true);
        sendButton->setEnabled(false);
        isConnected = false;
    }
}
