#include "start.h"
#include "UserManager.h"
#include <QVBoxLayout>
#include <QMessageBox>

UserManager userManager;

start::start(QWidget *parent):QWidget(parent)
{
    // 创建按钮
    backButton = new BUTTON("返回", this);
    backButton->move(0, 0);

    // 创建账号和密码输入框
    accountLineEdit = new QLineEdit(this);
    accountLineEdit->setPlaceholderText("请输入账号");
    accountLineEdit->setFixedSize(200,50);
    accountLineEdit->move(700,400);

    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("请输入密码");
    passwordLineEdit->setEchoMode(QLineEdit::Password); // 设置密码框隐藏输入的文字
    passwordLineEdit->move(700,500);
    passwordLineEdit->setFixedSize(200,50);

    // 创建显示密码按钮
    showPasswordButton = new BUTTON("显示密码", this);
    showPasswordButton->move(910, 500); // 按钮放在密码框右侧

    // 连接按钮的点击事件到槽函数
    connect(showPasswordButton, &BUTTON::clicked, this, [this]() {
        // 切换密码框的显示模式
        if (passwordLineEdit->echoMode() == QLineEdit::Password) {
            passwordLineEdit->setEchoMode(QLineEdit::Normal); // 显示密码
            showPasswordButton->setText("隐藏密码");
        } else {
            passwordLineEdit->setEchoMode(QLineEdit::Password); // 隐藏密码
            showPasswordButton->setText("显示密码");
        }
    });

    // 创建其他按钮
    registerButton = new BUTTON("注册", this);
    loginButton = new BUTTON("登录", this);
    guestButton = new BUTTON("游客模式", this);
    registerButton->move(0,250);
    loginButton->move(0,500);
    guestButton->move(0,750);

    connect(loginButton, &QPushButton::clicked, this, &start::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &start::onRegisterClicked);

    //MsgBox
    YESmessageBox = new MessageBox();
    NOmessageBox = new MessageBox();
    connect(NOmessageBox->closeButton, &BUTTON::clicked, this, [this]() {
        NOmessageBox->accept();  // 调用 QMessageBox 的 accept
    });
}

void start::onLoginClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();


    if (username.isEmpty() || password.isEmpty()) {
        NOmessageBox->setMessage("账号和密码不能为空！");
        NOmessageBox->exec();
        return;
    }

    userManager.loadFromFile();

    // 在 userManager 中验证账号密码
    if (userManager.checkPassword(username, password)) {
        YESmessageBox->setMessage("成功登录！");
        YESmessageBox->exec();
        // 登录成功后的处理，比如跳转到主界面
    }
    else {
        NOmessageBox->setMessage("账号或密码错误！");
        NOmessageBox->exec();
    }
}

void start::onRegisterClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        NOmessageBox->setMessage("账号和密码不能为空！");
        NOmessageBox->exec();
        //QMessageBox::warning(this, "输入错误", "账号和密码不能为空！");
        return;
    }

    // 加载用户信息
    userManager.loadFromFile();

    // 在 userManager 中检查账号是否已存在
    if (userManager.getAllUserIds().contains(username)) {
        NOmessageBox->setMessage("该账号已存在！");
        NOmessageBox->exec();
        //QMessageBox::warning(this, "注册失败", "该账号已存在！");
        return;
    }

    // 添加新用户
    userManager.addUser(username, password);
    userManager.saveToFile();  // 保存到文件

    NOmessageBox->setMessage("账号注册成功！");
    NOmessageBox->exec();
    //QMessageBox::information(this, "注册成功", "账号注册成功！");
    // 注册成功后的处理，比如跳转到登录界面
}


start::~start()
{
    //如果 QObject（或其子类，如 QWidget、QPushButton 等）有 parent,对象树会自动管理生命周期，不需要手动 delete

}
