#ifndef START_H
#define START_H

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "widget.h"
#include "button.h"

class start :public Widget{
    Q_OBJECT

public:
    start(Widget *parent = nullptr);

    ~start();
private:
    button* backbutton;
    QStackedWidget *stack;
    button *startButton;
    button *aboutButton;
    button *settingsButton;
    button *exitButton;
};

#endif // START_H
