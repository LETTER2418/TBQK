#include "onlinemode.h"
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "socketmanager.h"

OnlineMode::OnlineMode(QWidget *parent, SocketManager* manager)
    : QWidget(parent),
    socketManager(manager)
{
    // 创建使用Lbutton的按钮
    createRoomButton = new Lbutton(this, "创建房间");
    joinRoomButton = new Lbutton(this, "加入房间");
    
    // 创建水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addWidget(createRoomButton, 0, Qt::AlignCenter);
    mainLayout->addSpacing(20);  // 按钮之间的间距
    mainLayout->addWidget(joinRoomButton, 0, Qt::AlignCenter);
    mainLayout->addStretch(1);
    
    // 创建 msgBox, 传递 socketManager
    // OnlineMsgBox 的构造函数是 OnlineMsgBox(QWidget *parent = nullptr, SocketManager* manager = nullptr)
    // 如果 socketManager 在此时可能为 nullptr，需要考虑 msgBox 的行为
    // 或者确保 setSocketManager 在 OnlineMode 可见或使用前被调用
    msgBox = new OnlineMsgBox(this, this->socketManager);
    
    // 连接信号和槽
    connect(createRoomButton, &QPushButton::clicked, this, &OnlineMode::onCreateRoom);
    connect(joinRoomButton, &QPushButton::clicked, this, &OnlineMode::onJoinRoom);
}

void OnlineMode::onCreateRoom()
{
    msgBox->setMode(OnlineMsgBox::CreateMode);  // 设置为创建房间模式
    msgBox->show();
}

void OnlineMode::onJoinRoom()
{
    msgBox->setMode(OnlineMsgBox::JoinMode);  // 设置为加入房间模式
    msgBox->show();
}

 
