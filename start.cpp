#include "start.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>

Start::Start(QWidget *parent, DataManager *dataManager_, SocketManager *socketManager_) : QWidget(parent), dataManager(dataManager_), socketManager(socketManager_)
{
    // 创建按钮和输入框
    backButton = new Lbutton(this, "返回");
    loginButton = new Lbutton(this, "登录");
    loginButton->enableClickEffect(true);
    registerButton = new Lbutton(this, "注册");
    registerButton->enableClickEffect(true);
    guestButton = new Lbutton(this, "游客模式");
    showPasswordButton = new Lbutton(this, "显示密码");
    showPasswordButton->enableClickEffect(true);

    accountLineEdit = new QLineEdit(this);
    accountLineEdit->setPlaceholderText("请输入账号");
    accountLineEdit->setText("1"); // 设置默认文本为"1"
    accountLineEdit->setFixedSize(200, 50);

    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("请输入密码");
    passwordLineEdit->setEchoMode(QLineEdit::Password); // 设置密码框隐藏输入的文字
    passwordLineEdit->setText("1");                     // 设置默认文本为"1"
    passwordLineEdit->setFixedSize(200, 50);

    // 创建主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建GroupBox
    QFont groupBoxFont;
    groupBoxFont.setPointSize(16);

    QGroupBox *inputGroupBox = new QGroupBox("注册登录面板", this);
    inputGroupBox->setFont(groupBoxFont);
    inputGroupBox->setStyleSheet("QGroupBox { color: white; border: 1px solid white; border-radius: 5px; margin-top: 15px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 10px 0 10px; }");
    inputGroupBox->setFixedSize(300, 400);

    // GroupBox内部布局
    QVBoxLayout *groupBoxLayout = new QVBoxLayout(inputGroupBox);
    groupBoxLayout->setContentsMargins(20, 40, 20, 40);
    groupBoxLayout->setSpacing(30);
    groupBoxLayout->setAlignment(Qt::AlignHCenter);

    groupBoxLayout->addStretch(1);
    groupBoxLayout->addWidget(accountLineEdit, 0, Qt::AlignHCenter);
    groupBoxLayout->addWidget(passwordLineEdit, 0, Qt::AlignHCenter);
    groupBoxLayout->addWidget(showPasswordButton, 0, Qt::AlignHCenter);
    groupBoxLayout->addStretch(1);

    // 创建左侧按钮布局
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(guestButton);

    // 添加到主布局
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(10);
    mainLayout->addWidget(inputGroupBox, 0, Qt::AlignCenter);
    mainLayout->addStretch(12);

    // 连接按钮的点击事件到槽函数
    connect(showPasswordButton, &Lbutton::clicked, this, [this]()
            {
        // 切换密码框的显示模式
        if (passwordLineEdit->echoMode() == QLineEdit::Password)
            {
                passwordLineEdit->setEchoMode(QLineEdit::Normal); // 显示密码
                showPasswordButton->setText("隐藏密码");
            }
        else
            {
                passwordLineEdit->setEchoMode(QLineEdit::Password); // 隐藏密码
                showPasswordButton->setText("显示密码");
            } });

    connect(loginButton, &QPushButton::clicked, this, &Start::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &Start::onRegisterClicked);
    connect(guestButton, &QPushButton::clicked, this, &Start::onGuestClicked);

    // MsgBox
    YESmessageBox = new MessageBox(this);
    NOmessageBox = new MessageBox(this);
    connect(NOmessageBox->closeButton, &QPushButton::clicked, this, [this]()
            {
                NOmessageBox->accept(); // 调用 QMessageBox 的 accept
            });
}

void Start::onLoginClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        NOmessageBox->setMessage("账号和密码不能为空！");
        NOmessageBox->exec();
        return;
    }

    // 验证账号密码
    if (dataManager->checkPassword(username, password))
    {
        YESmessageBox->setMessage("成功登录！");
        YESmessageBox->exec();
        currentUserId = username;
        dataManager->setCurrentUserId(username); // 使用setter方法设置当前用户ID
        socketManager->setLocalUserId(username);

        // 检查是否有临时头像需要保存
        if (!dataManager->getAvatarPath().isEmpty())
        {
            // 加载临时头像
            QPixmap tempAvatar(dataManager->getAvatarPath());
            if (!tempAvatar.isNull())
            {
                // 保存头像文件
                dataManager->saveAvatarFile(username, tempAvatar);
            }
        }
    }
    else
    {
        NOmessageBox->setMessage("账号或密码错误！");
        NOmessageBox->exec();
    }
}

void Start::onRegisterClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username == "系统")
    {
        NOmessageBox->setMessage("账号不符合规范！");
        NOmessageBox->exec();
        return;
    }

    if (username.isEmpty() || password.isEmpty())
    {
        NOmessageBox->setMessage("账号和密码不能为空！");
        NOmessageBox->exec();
        return;
    }

    // 检查账号是否已存在
    if (dataManager->getAllUserIds().contains(username))
    {
        NOmessageBox->setMessage("该账号已存在！");
        NOmessageBox->exec();
        return;
    }

    // 添加新用户
    dataManager->addUser(username, password);
    dataManager->saveToFile(); // 保存到文件

    NOmessageBox->setMessage("账号注册成功！");
    NOmessageBox->exec();
}

void Start::onGuestClicked()
{
    QString username = "tourist";
    QString password = "tourist123";

    // 如果游客账号不存在，先创建游客账号
    if (!dataManager->getAllUserIds().contains(username))
    {
        dataManager->addUser(username, password);
        dataManager->saveToFile(); // 保存到文件
    }

    // 自动登录游客账号
    YESmessageBox->setMessage("以游客身份登录！");
    YESmessageBox->exec();
    currentUserId = username;
    dataManager->setCurrentUserId(username);
    socketManager->setLocalUserId(username);

    // 检查是否有临时头像需要保存
    if (!dataManager->getAvatarPath().isEmpty())
    {
        // 加载临时头像
        QPixmap tempAvatar(dataManager->getAvatarPath());
        if (!tempAvatar.isNull())
        {
            // 保存头像文件
            dataManager->saveAvatarFile(username, tempAvatar);
        }
    }
}

Start::~Start()
{
}
