#include "about.h"
#include <QVBoxLayout>

about::about(QWidget *parent):QWidget(parent)
{
    backButton = new button("返回", this);
    backButton->move(0, 0);

    QVBoxLayout *layout = new QVBoxLayout(this);

    button *loginButton = new button("Login", this);
    button *registerButton = new button("Register", this);
    button *guestButton = new button("Guest", this);

    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(guestButton);

    this->setLayout(layout);
}

about::~about()
{

}
