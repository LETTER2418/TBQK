#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "Widget.h"
#include "button.h"

class WindowManager :public Widget{
    Q_OBJECT

public:
    WindowManager(Widget *parent = nullptr);

    ~WindowManager();
private:
    button* backbutton;
    QStackedWidget *stack;
    button *startButton;
    button *aboutButton;
    button *settingsButton;
    button *exitButton;
};

#endif // WINDOWMANAGER_H
