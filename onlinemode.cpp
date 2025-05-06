#include "onlinemode.h"
#include <QHBoxLayout>
#include <QMessageBox>

OnlineMode::OnlineMode(QWidget *parent)
    : QWidget(parent)
{
    // 创建使用Lbutton的按钮
    createRoomButton = new Lbutton(this, "创建房间");
    joinRoomButton = new Lbutton(this, "加入房间");
    
    // 创建垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addWidget(createRoomButton, 0, Qt::AlignCenter);
    mainLayout->addSpacing(20);  // 按钮之间的间距
    mainLayout->addWidget(joinRoomButton, 0, Qt::AlignCenter);
    mainLayout->addStretch(1);
    
    // 连接信号和槽
    connect(createRoomButton, &QPushButton::clicked, this, &OnlineMode::onCreateRoom);
    connect(joinRoomButton, &QPushButton::clicked, this, &OnlineMode::onJoinRoom);
    
}

void OnlineMode::onCreateRoom()
{
    // 临时实现，后续可以连接到实际创建房间的功能
    QMessageBox::information(this, "提示", "创建房间功能正在开发中...");
}

void OnlineMode::onJoinRoom()
{
    // 临时实现，后续可以连接到实际加入房间的功能
    QMessageBox::information(this, "提示", "加入房间功能正在开发中...");
}


 
