#include "about.h"
#include <QVBoxLayout>

About::About(QWidget *parent): QWidget(parent)
{
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);

}

About::~About()
{

}
