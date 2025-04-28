#include "Menu.h"
#include <QBoxLayout>

Menu::Menu(QWidget *parent):QWidget(parent)
{
    logoutButton = new Lbutton("注销", this);
    logoutButton -> move(0, 0);
    levelEditorButton = new Lbutton("关卡编辑", this);
    levelModeButton = new Lbutton("关卡模式", this);
    rankButton = new Lbutton("排行榜", this);
    onlineButton = new Lbutton("联机模式", this);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(levelEditorButton, 0, 0);
    layout->addWidget(levelModeButton, 1, 0);
    layout->addWidget(rankButton, 0, 1);
    layout->addWidget(onlineButton, 1, 1);
}

Menu::~Menu()
{

}
