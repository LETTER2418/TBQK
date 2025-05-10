#include "onlinechat.h"
#include <QDateTime> // For timestamping messages
#include <QFileInfo>

OnlineChat::OnlineChat(SocketManager* manager, QWidget *parent)
    : QWidget{parent},
      socketManager(manager) // Initialize socketManager
{
    this->setGeometry(20, 450, 300, 400);
    
    // Create UI elements
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);

    messageInput = new QLineEdit(this);
    sendButton = new Lbutton(this,"发送");
    messageInput->setFixedHeight(sendButton->height());

    // Layouts
    inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chatDisplay);
    mainLayout->addLayout(inputLayout);

    setLayout(mainLayout);

    // Connections
    connect(socketManager, &SocketManager::newMessageReceived, this, &OnlineChat::displayMessage);
    connect(sendButton, &QPushButton::clicked, this, &OnlineChat::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &OnlineChat::sendMessage);

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile()) {
        // 这个背景感觉不如按原始背景作为背景，后面有需要再加上
        //backgroundImage.load(backgroundImagePath);
    } else {
        qWarning() << "Image file not found: " << backgroundImagePath;
    }
}

OnlineChat::~OnlineChat()
{
   
}

void OnlineChat::displayMessage(const QString& userId, const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    chatDisplay->append(QString("%1 <%2>: %3").arg(timestamp).arg(userId).arg(message));
}

void OnlineChat::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull()) {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}

void OnlineChat::sendMessage()
{
        QString message = messageInput->text().trimmed();
        if (!message.isEmpty()) {
            // 获取本地用户ID
            QString localUserId = socketManager->getLocalUserId();
            if (localUserId.isEmpty()) {
                localUserId = "User"; // 如果获取不到 UserId，使用默认值
            }

            socketManager->SendChatMessage(message, localUserId); 
            // 立即在本地显示自己发送的消息
            displayMessage(localUserId, message);
            messageInput->clear();
        }
}
