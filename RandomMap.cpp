#include "RandomMap.h"
#include <QBoxLayout>

RandomMap::RandomMap(QWidget *parent):QWidget(parent)
{
    backButton = new BUTTON("返回", this);
    backButton -> move(0, 0);

}

RandomMap::~RandomMap()
{

}
