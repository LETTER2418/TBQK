#include "levelmode.h"
#include <QGridLayout>     // 确保 QGridLayout 被包含，尽管h文件里有了，cpp中也加上以防万一
#include <QSizePolicy>
#include <QString> //  需要 QString

LevelMode::LevelMode(QWidget *parent)
    : QWidget(parent)
{
    // 主布局
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 设置主布局边距为0
    mainLayout->setVerticalSpacing(10);          // 设置主布局内垂直间距
    mainLayout->setHorizontalSpacing(0);        // 设置主布局内水平间距为0 (影响第一行 backButton 和右侧空间的间距)

    // 返回按钮
    backButton = new Lbutton(this, "↩️ 返回");
    backButton->move(0, 0);

    // 关卡按钮布局
    QGridLayout *levelButtonsLayout = new QGridLayout();
    levelButtonsLayout->setContentsMargins(0, 0, 0, 0); // 设置关卡按钮布局边距为0
    levelButtonsLayout->setVerticalSpacing(100);   // 单独设置垂直间距
    levelButtonsLayout->setHorizontalSpacing(10);  // 单独设置水平间距 (您可以按需调整此值)

    // 创建并添加按钮
    for (int i = 0; i < 12; ++i)
        {
            buttons[i] = new Lbutton(this, QString("第%1关").arg(i + 1));
            int row = i / 4;
            int col = i % 4;
            levelButtonsLayout->addWidget(buttons[i], row, col);
        }

    // 设置关卡按钮布局的行和列的拉伸因子
    for (int i = 0; i < 3; ++i)   // 3行
        {
            levelButtonsLayout->setRowStretch(i, 1);
        }
    for (int i = 0; i < 4; ++i)   // 4列
        {
            levelButtonsLayout->setColumnStretch(i, 1);
        }

    mainLayout->addLayout(levelButtonsLayout, 1, 0, 1, 4); // 关卡按钮布局在第二行，跨4列
    mainLayout->setRowStretch(0, 0); // 为mainLayout的第0行（空行）设置不拉伸
    mainLayout->setRowStretch(1, 1); // 为mainLayout的第1行（关卡按钮区域）设置拉伸

    // 为主布局的列设置拉伸因子
    for (int i = 0; i < 4; ++i)
        {
            mainLayout->setColumnStretch(i, 1);
        }

    setLayout(mainLayout); // 应用主布局
}

LevelMode::~LevelMode()
{
    // delete ui; // 移除 delete ui
    // QLayout 会自动删除其管理的 QLayoutItem 和子 QLayout，以及设置了 Qt::WA_DeleteOnClose 属性的 QWidget
    // buttons 数组中的按钮作为子 widget 添加到布局中，并且父对象是 this (LevelMode widget)，
    // 当 LevelMode 析构时，其子对象会被自动删除。backButton 同理。
    // 所以这里通常不需要手动 delete buttons[i] 和 backButton。
}
