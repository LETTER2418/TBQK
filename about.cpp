#include "about.h"
#include <QVBoxLayout>

About::About(QWidget *parent):QWidget(parent)
{
    backButton = new BUTTON("返回", this);
    backButton->move(0, 0);


}

About::~About()
{

}
