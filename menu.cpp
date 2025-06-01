#include "menu.h"
#include <QBoxLayout>

Menu::Menu(QWidget *parent) : QWidget(parent)
{
    logoutButton = new Lbutton(this, "↩️ 注销");
    logoutButton->move(0, 0);
    levelEditorButton = new Lbutton(this, "✏️ 关卡编辑");
    levelModeButton = new Lbutton(this, "🎮 关卡模式");
    rankButton = new Lbutton(this, "🏆 排行榜");
    onlineButton = new Lbutton(this, "🌐 联机模式");

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
