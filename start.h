#ifndef START_H
#define START_H
#include <QWidget>
#include "button.h"

class start :public QWidget{
public:
    start();
private:
    button* backbutton;
};

#endif // START_H
