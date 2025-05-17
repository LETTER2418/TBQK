#include "start.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

Start::Start(QWidget *parent, DataManager *dataManager_, SocketManager *socketManager_):QWidget(parent), dataManager(dataManager_), socketManager(socketManager_)
{
    // 创建按钮和输入框
    backButton = new Lbutton(this, "返回");
    loginButton = new Lbutton(this, "登录");
    registerButton = new Lbutton(this, "注册");
    guestButton = new Lbutton(this, "游客模式");
    showPasswordButton = new Lbutton(this, "显示密码");
    
    accountLineEdit = new QLineEdit(this);
    accountLineEdit->setPlaceholderText("请输入账号");
    accountLineEdit->setText("1"); // 设置默认文本为"1"
    accountLineEdit->setFixedSize(200, 50);
    
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("请输入密码");
    passwordLineEdit->setEchoMode(QLineEdit::Password); // 设置密码框隐藏输入的文字
    passwordLineEdit->setText("1"); // 设置默认文本为"1"
    passwordLineEdit->setFixedSize(200, 50);

    // 创建主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    // 创建左侧按钮布局
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch(1); 
    buttonLayout->addWidget(registerButton);
    buttonLayout->addStretch(1); 
    buttonLayout->addWidget(loginButton);
    buttonLayout->addStretch(1); 
    buttonLayout->addWidget(guestButton);
    
    // 创建中央输入框布局
    QVBoxLayout *inputLayout = new QVBoxLayout();
    inputLayout->addStretch();
    
    // 账号输入框布局
    QHBoxLayout *accountLayout = new QHBoxLayout();
    accountLayout->addWidget(accountLineEdit);
    accountLayout->addStretch();
    
    // 密码输入框布局
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(passwordLineEdit);
    passwordLayout->addWidget(showPasswordButton);
    passwordLayout->addStretch();
    
    inputLayout->addLayout(accountLayout);
    inputLayout->addLayout(passwordLayout);
    inputLayout->addStretch();
    
    // 添加布局到主布局
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(inputLayout);
    mainLayout->addStretch(1);

    // 连接按钮的点击事件到槽函数
    connect(showPasswordButton, &Lbutton::clicked, this, [this]() {
        // 切换密码框的显示模式
        if (passwordLineEdit->echoMode() == QLineEdit::Password) {
            passwordLineEdit->setEchoMode(QLineEdit::Normal); // 显示密码
            showPasswordButton->setText("隐藏密码");
        } else {
            passwordLineEdit->setEchoMode(QLineEdit::Password); // 隐藏密码
            showPasswordButton->setText("显示密码");
        }
    });

    connect(loginButton, &QPushButton::clicked, this, &Start::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &Start::onRegisterClicked);

    //MsgBox
    YESmessageBox = new MessageBox(this);
    NOmessageBox = new MessageBox(this);
    connect(NOmessageBox->closeButton, &Lbutton::clicked, this, [this]() {
        NOmessageBox->accept();  // 调用 QMessageBox 的 accept
    });
}

void Start::onLoginClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        NOmessageBox->setMessage("账号和密码不能为空！");
        NOmessageBox->exec();
        return;
    }

    // 验证账号密码
    if (dataManager->checkPassword(username, password)) {
        YESmessageBox->setMessage("成功登录！");
        YESmessageBox->exec();
        currentUserId = username;
        dataManager->setCurrentUserId(username); // 使用setter方法设置当前用户ID
        socketManager->setLocalUserId(username);
        
        // 检查是否有临时头像需要保存
        if (!dataManager->getAvatarPath().isEmpty()) {
            // 加载临时头像
            QPixmap tempAvatar(dataManager->getAvatarPath());
            if (!tempAvatar.isNull()) {
                // 保存头像文件
                dataManager->saveAvatarFile(username, tempAvatar);
            }
        }
    }
    else {
        NOmessageBox->setMessage("账号或密码错误！");
        NOmessageBox->exec();
    }
}

void Start::onRegisterClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username=="系统") {
        NOmessageBox->setMessage("账号不符合规范！");
        NOmessageBox->exec();
        return;
    }

    if (username.isEmpty() || password.isEmpty()) {
        NOmessageBox->setMessage("账号和密码不能为空！");
        NOmessageBox->exec();
        return;
    }

    // 检查账号是否已存在
    if (dataManager->getAllUserIds().contains(username)) {
        NOmessageBox->setMessage("该账号已存在！");
        NOmessageBox->exec();
        return;
    }

    // 添加新用户
    dataManager->addUser(username, password);
    dataManager->saveToFile();  // 保存到文件

    NOmessageBox->setMessage("账号注册成功！");
    NOmessageBox->exec();

}


Start::~Start()
{
}
