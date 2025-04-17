#include "About.h"
#include <QVBoxLayout>

About::About(QWidget *parent):QWidget(parent)
{
    backButton = new Lbutton("返回", this);
    backButton->move(0, 0);


}

About::~About()
{

}
