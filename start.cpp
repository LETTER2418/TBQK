#include "start.h"
#include "UserManager.h"
#include "MessageBox.h"
#include <QVBoxLayout>
#include <QMessageBox>

UserManager userManager;

start::start(QWidget *parent):QWidget(parent)
{
    // 创建按钮
    backButton = new button("返回", this);
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

    // 创建其他按钮
    registerButton = new button("Register", this);
    loginButton = new button("Login", this);
    guestButton = new button("Guest", this);
    registerButton->move(0,250);
    loginButton->move(0,500);
    guestButton->move(0,750);

    connect(loginButton, &QPushButton::clicked, this, &start::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &start::onRegisterClicked);

}

void start::onLoginClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        MessageBox messageBox;
        messageBox.setMessage("账号和密码不能为空！");
        messageBox.exec();
        //QMessageBox::warning(this, "输入错误", "账号和密码不能为空！");
        return;
    }

    // 在 userManager 中验证账号密码
    if (userManager.checkPassword(username, password)) {
        QMessageBox::information(this, "登录成功", "欢迎回来，" + username);
        // 登录成功后的处理，比如跳转到主界面
    } else {
        QMessageBox::warning(this, "登录失败", "账号或密码错误！");
    }
}

void start::onRegisterClicked()
{
    QString username = accountLineEdit->text();
    QString password = passwordLineEdit->text();


    if (username.isEmpty() || password.isEmpty()) {

        QMessageBox::warning(this, "输入错误", "账号和密码不能为空！");
        return;
    }

    // 在 userManager 中检查账号是否已存在
    if (userManager.getAllUserIds().contains(username)) {
        QMessageBox::warning(this, "注册失败", "该账号已存在！");
        return;
    }

    // 添加新用户
    userManager.addUser(username, password);
    userManager.saveToFile();  // 保存到文件

    QMessageBox::information(this, "注册成功", "账号注册成功！");
    // 注册成功后的处理，比如跳转到登录界面
}


start::~start()
{
    //如果 QObject（或其子类，如 QWidget、QPushButton 等）有 parent,对象树会自动管理生命周期，不需要手动 delete

}
