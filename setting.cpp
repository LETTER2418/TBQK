#include "setting.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

Setting::Setting(QWidget *parent, DataManager *dataManager_)
    : QWidget(parent), dataManager(dataManager_)
{
    // 设置应用图标
    QIcon appIcon(":/image/taiji.png");
    this->setWindowIcon(appIcon);

    // 创建返回按钮
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);

    // 创建标题标签
    QLabel *titleLabel = new QLabel("设置", this);
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    // 创建清除数据按钮
    clearDataButton = new Lbutton(this, "清除所有数据");
    
    // 创建消息框
    confirmMessageBox = new MessageBox(this, true);
    confirmMessageBox->setMessage("确定要清除所有数据吗？这将删除所有用户、地图和排行榜数据，且无法恢复！");
    
    successMessageBox = new MessageBox(this);
    successMessageBox->setMessage("所有数据已清除！");

    // 连接清除数据按钮点击事件
    connect(clearDataButton, &Lbutton::clicked, this, [this]() {
        confirmMessageBox->exec();
    });

    // 连接确认对话框的确认按钮
    connect(confirmMessageBox->closeButton, &Lbutton::clicked, this, [this]() {
        dataManager->clearAllData();
        confirmMessageBox->accept();
        successMessageBox->exec();
    });

    // 连接确认对话框的取消按钮
    connect(confirmMessageBox->cancelButton, &Lbutton::clicked, this, [this]() {
        confirmMessageBox->reject(); // 关闭对话框但不执行任何清除操作
    });

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(clearDataButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    
    // 设置边距
    mainLayout->setContentsMargins(50, 80, 50, 50);
}

Setting::~Setting()
{
    delete confirmMessageBox;
    delete successMessageBox;
}
