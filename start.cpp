#include "start.h"

start::start(QWidget *parent):QWidget(parent)
{
    backButton = new button("返回", this);
    backButton->move(0, 0);
}

start::~start()
{
    delete backButton;
}
