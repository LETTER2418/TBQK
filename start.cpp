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
    delete backButton;
    delete loginButton;
    delete registerButton;
    delete guestButton;
}
