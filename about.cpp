#include "about.h"
#include <QVBoxLayout>

about::about(QWidget *parent):QWidget(parent)
{
    backButton = new button("返回", this);
    backButton->move(0, 0);


}

about::~about()
{

}
