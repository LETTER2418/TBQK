#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QStackedWidget>
#include <QVBoxLayout>
#include "Widget.h"
#include "Lbutton.h"

class WindowManager :public Widget{
    Q_OBJECT

public:
    WindowManager(Widget *parent = nullptr);

    ~WindowManager();
private:
    Lbutton* backbutton;
    QStackedWidget *pageStack;
    Lbutton *startButton;
    Lbutton *aboutButton;
    Lbutton *settingsButton;
    Lbutton *exitButton;
};

#endif // WINDOWMANAGER_H
