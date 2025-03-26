#include "start.h"
#include <QVBoxLayout>

start::start(QWidget *parent):QWidget(parent)
{
    backButton = new button("返回", this);
    backButton->move(0, 0);

    QVBoxLayout *layout = new QVBoxLayout(this);

    loginButton = new button("Login", this);
    registerButton = new button("Register", this);
    guestButton = new button("Guest", this);

    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(guestButton);

    this->setLayout(layout);
}

start::~start()
{
    //如果 QObject（或其子类，如 QWidget、QPushButton 等）有 parent,对象树会自动管理生命周期，不需要手动 delete

}
