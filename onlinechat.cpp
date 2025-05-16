#include "onlinechat.h"
#include <QDateTime> // For timestamping messages
#include <QFileInfo>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QDesktopServices>
#include <QFontMetrics>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>
#include <QShowEvent>
#include <QTcpSocket>

// ChatBubble 实现
ChatBubble::ChatBubble(const QString &text, bool isSelf, QPixmap avatar, QWidget *parent)
    : QFrame(parent), isSelf(isSelf), avatar(avatar), userId("")
{
    setFrameShape(QFrame::NoFrame);

    // 创建布局
    QHBoxLayout *layout = new QHBoxLayout(this);

    // 创建头像标签
    avatarLabel = new QLabel(this);
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setScaledContents(true);
    avatarLabel->setPixmap(avatar);

    // 创建消息标签
    messageLabel = new QLabel(text, this);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // 设置气泡样式
    QString bgColor = isSelf ? "#95EC69" : "#FFFFFF"; // 自己的消息是绿色，他人的是白色
    messageLabel->setStyleSheet(QString("QLabel { background-color: %1; border-radius: 10px; padding: 10px; }").arg(bgColor));

    // 设置最大宽度
    QFontMetrics fm(messageLabel->font());
    int textWidth = fm.horizontalAdvance(text);
    int maxWidth = 200; // 气泡最大宽度

    if (textWidth > maxWidth)
        {
            messageLabel->setFixedWidth(maxWidth);
        }
    else
        {
            messageLabel->setMinimumWidth(textWidth + 20); // 添加一些边距
        }

    // 根据是否是自己的消息来布局
    if (isSelf)
        {
            layout->addStretch();
            layout->addWidget(messageLabel);
            layout->addWidget(avatarLabel);
        }
    else
        {
            layout->addWidget(avatarLabel);
            layout->addWidget(messageLabel);
            layout->addStretch();
        }

    setLayout(layout);
}

void ChatBubble::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QFrame::paintEvent(event);
}

OnlineChat::OnlineChat(SocketManager* manager, DataManager* dm, QWidget *parent)
    : QWidget(parent), socketManager(manager), dataManager(dm)
{
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->resize(800, 600);

    // 加载默认头像
    defaultAvatar = QPixmap(40, 40);
    defaultAvatar.fill(Qt::white);

    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建顶部面板 - 用户信息
    topPanel = new QWidget(this);
    topPanel->setFixedHeight(60);
    topPanel->setStyleSheet("background-color: #F5F5F5;");

    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);

    remoteUserLabel = new QLabel("对方用户", topPanel);
    remoteUserLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    topLayout->addWidget(remoteUserLabel, 0, Qt::AlignCenter);

    // 添加顶部面板到主布局
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(createHLine());

    // 创建中间面板 - 聊天内容
    chatScrollArea = new QScrollArea(this);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setStyleSheet("background-color: #F5F5F5; border: none;");
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    chatContentWidget = new QWidget(chatScrollArea);
    chatContentLayout = new QVBoxLayout(chatContentWidget);
    chatContentLayout->setAlignment(Qt::AlignTop);
    chatContentLayout->setSpacing(15);
    chatContentLayout->setContentsMargins(10, 10, 10, 10);

    chatScrollArea->setWidget(chatContentWidget);

    // 添加中间面板到主布局
    mainLayout->addWidget(chatScrollArea, 1);
    mainLayout->addWidget(createHLine());

    // 创建底部面板 - 输入区
    bottomPanel = new QWidget(this);
    bottomPanel->setFixedHeight(150);
    bottomPanel->setStyleSheet("background-color: #F5F5F5;");

    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomPanel);

    // 输入框
    messageInput = new QTextEdit(bottomPanel);
    messageInput->setPlaceholderText("请输入消息...");
    messageInput->setStyleSheet("border: 1px solid #CCCCCC; background-color: white; border-radius: 5px;");

    // 按钮布局
    buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 5, 0, 5);

    // 发送按钮
    sendButton = new Lbutton(bottomPanel, "发送");
    sendButton->setStyleSheet("QPushButton {""color:black""}");

    buttonLayout->addStretch();
    buttonLayout->addWidget(sendButton);

    bottomLayout->addWidget(messageInput);
    bottomLayout->addLayout(buttonLayout);

    // 添加底部面板到主布局
    mainLayout->addWidget(bottomPanel);

    setLayout(mainLayout);

    // Connections
    connect(socketManager, &SocketManager::newMessageReceived, this, &OnlineChat::displayMessage);
    connect(socketManager, &SocketManager::avatarImageReceived, this, &OnlineChat::onAvatarImageReceived);
    connect(sendButton, &QPushButton::clicked, this, &OnlineChat::sendMessage);
}

OnlineChat::~OnlineChat()
{
}

// 创建水平分隔线
QFrame* OnlineChat::createHLine()
{
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #CCCCCC;");
    line->setFixedHeight(1);
    return line;
}

// 加载用户头像
QPixmap OnlineChat::loadAvatar(const QString& userId)
{
    // 检查是否已缓存头像
    if (userAvatars.contains(userId))
        {
            return userAvatars[userId];
        }
    
    // 如果dataManager可用，使用它加载头像
    if (dataManager) {
        QPixmap avatar = dataManager->loadAvatarFile(userId);
        if (!avatar.isNull()) {
            // 缓存并返回加载的头像
            userAvatars[userId] = avatar.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            return userAvatars[userId];
        }
    }
    
    // 使用默认白色头像
    userAvatars[userId] = defaultAvatar;
    return defaultAvatar;
}

// 从Setting类的配置文件加载头像
QPixmap OnlineChat::loadAvatarFromSettings()
{
    // 如果DataManager可用，从DataManager获取头像文件
    if (dataManager)
        {
            QString userId = socketManager->getLocalUserId();
            if (!userId.isEmpty())
                {
                    // 直接从文件加载头像
                    QPixmap avatar = dataManager->loadAvatarFile(userId);
                    if (!avatar.isNull())
                        {
                            return avatar.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        }
                }
        }

    // 无法加载头像，返回默认白色头像
    return defaultAvatar;
}

// 设置用户自定义头像
void OnlineChat::setUserAvatar(const QString& userId)
{
    // 从设置中加载
    QPixmap settingsAvatar = loadAvatarFromSettings();
    if (!settingsAvatar.isNull())
        {
            userAvatars[userId] = settingsAvatar;
            // 如果设置的是当前用户的头像，发送给对方
            if (socketManager && userId == socketManager->getLocalUserId())
                {
                    socketManager->SendAvatarImage(userAvatars[userId], userId);
                }
        }
    else
        {
            // 设置为默认白色头像
            userAvatars[userId] = defaultAvatar;
            if (socketManager && userId == socketManager->getLocalUserId())
                {
                    socketManager->SendAvatarImage(defaultAvatar, userId);
                }
        }
}

void OnlineChat::displayMessage(const QString& userId, const QString& message, bool isSelfMessage)
{
    QString localUserId = socketManager->getLocalUserId();
    
    // 处理空消息 - 用于用户ID交换
    if (message.isEmpty())
        {
            // 如果不是自己的ID，则更新对方用户名
            if (userId != localUserId && socketManager)
                {
                    remoteUserLabel->setText(userId);
                }
            return;
        }
    
    // 处理非空消息 - 正常聊天内容
    // 使用传入的isSelfMessage参数确定消息来源
    bool isSelf = isSelfMessage;
    
    // 加载合适的头像 - 确保使用正确的头像
    QPixmap avatar;
    if (isSelf)
        {
            // 如果是自己发送的消息，使用自己的头像
            avatar = loadAvatar(localUserId);
        }
    else
        {
            // 如果是对方发送的消息，使用对方的头像
            avatar = loadAvatar(userId);
        }
    
    // 创建聊天气泡，确保isSelf状态正确
    ChatBubble *bubble = new ChatBubble(message, isSelf, avatar, chatContentWidget);
    bubble->setUserId(userId); // 保存消息关联的用户ID
    chatContentLayout->addWidget(bubble);
    
    // 滚动到底部
    QScrollBar *scrollBar = chatScrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void OnlineChat::paintEvent(QPaintEvent *event)
{
    // 简单的纯色背景绘制
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#F5F5F5"));
    QWidget::paintEvent(event);
}

void OnlineChat::sendMessage()
{
    QString message = messageInput->toPlainText().trimmed();
    if (!message.isEmpty())
        {
            // 获取本地用户ID
            QString localUserId = socketManager->getLocalUserId();
            if (localUserId.isEmpty())
                {
                    localUserId = "User"; // 如果获取不到 UserId，使用默认值
                }

            // 先发送网络消息
            socketManager->SendChatMessage(message, localUserId);

            // 检查本地用户头像是否存在，如果不存在则加载
            if (!userAvatars.contains(localUserId))
                {
                    QPixmap avatar = loadAvatarFromSettings();
                    if (avatar.isNull())
                        {
                            avatar = defaultAvatar;
                        }
                    userAvatars[localUserId] = avatar;

                    // 发送头像给对方
                    socketManager->SendAvatarImage(avatar, localUserId);
                }

            // 立即在本地显示自己发送的消息 - 注意：这里不通过信号触发，而是直接调用displayMessage
            // 这样sender()就不是socketManager，可以在displayMessage中区分出本地发出的消息
            displayMessage(localUserId, message, true);
            messageInput->clear();
        }
}

// 发送当前用户的头像
void OnlineChat::sendCurrentUserAvatar()
{
    if (!socketManager)
        {
            return;
        }

    QString localUserId = socketManager->getLocalUserId();
    if (localUserId.isEmpty())
        {
            return;
        }

    // 检查是否已有当前用户的头像
    if (userAvatars.contains(localUserId))
        {
            // 发送已有头像给对方
            socketManager->SendAvatarImage(userAvatars[localUserId], localUserId);
        }
    else
        {
            // 尝试从设置中加载头像
            QPixmap avatar = loadAvatarFromSettings();
            if (!avatar.isNull())
                {
                    userAvatars[localUserId] = avatar;
                    socketManager->SendAvatarImage(avatar, localUserId);
                }
            else
                {
                    // 使用默认头像
                    userAvatars[localUserId] = defaultAvatar;
                    socketManager->SendAvatarImage(defaultAvatar, localUserId);
                }
        }
}

// 接收头像图片
void OnlineChat::onAvatarImageReceived(const QString& userId, const QPixmap& avatar)
{
    if (userId.isEmpty() || avatar.isNull())
        {
            return;
        }
    
    QString localUserId = socketManager->getLocalUserId();
   
    // 保存接收到的头像
    userAvatars[userId] = avatar;
    
    // 只在接收头像且有活跃连接时更新对方的用户名显示   
    if (socketManager && userId != localUserId && socketManager && !socketManager->getClientSockets().isEmpty())
        {
            remoteUserLabel->setText(userId);
        }
    
    // 更新所有相关聊天气泡的头像
    for (int i = 0; i < chatContentLayout->count(); ++i)
        {
            QWidget* widget = chatContentLayout->itemAt(i)->widget();
            ChatBubble* bubble = qobject_cast<ChatBubble*>(widget);
            if (bubble && bubble->getUserId() == userId)
                {
                    bubble->updateAvatar(avatar);
                }
        }
}

// 清空聊天历史记录
void OnlineChat::clearChatHistory()
{
    // 删除所有聊天气泡
    QLayoutItem *child;
    while ((child = chatContentLayout->takeAt(0)) != nullptr)
        {
            if (child->widget())
                {
                    delete child->widget();
                }
            delete child;
        }

    // 不添加系统通知，保持聊天界面完全清空
}

// 更新头像
void ChatBubble::updateAvatar(const QPixmap& newAvatar)
{
    if (newAvatar.isNull())
        {
            return;
        }

    avatar = newAvatar;
    avatarLabel->setPixmap(newAvatar);
}

// 当窗口显示时，通知对方我们的用户ID
void OnlineChat::announceUserPresence()
{
    if (!socketManager)
        {
            return;
        }
    
    QString localUserId = socketManager->getLocalUserId();
    if (localUserId.isEmpty())
        {
            return;
        }
    
    // 无论是服务端还是客户端，都发送自己的用户ID（空消息）
    socketManager->SendChatMessage("", localUserId);
    
    // 发送头像信息
    sendCurrentUserAvatar();
}

// 重写showEvent
void OnlineChat::showEvent(QShowEvent *event)
{
    // 调用基类实现
    QWidget::showEvent(event);

    // 当窗口显示时，发送用户信息
    QTimer::singleShot(500, this, &OnlineChat::announceUserPresence);
}


