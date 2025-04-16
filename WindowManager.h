#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "Widget.h"
#include "BUTTON.h"

class WindowManager :public Widget{
    Q_OBJECT

public:
    WindowManager(Widget *parent = nullptr);

    ~WindowManager();
private:
    BUTTON* backbutton;
    QStackedWidget *pageStack;
    BUTTON *startButton;
    BUTTON *aboutButton;
    BUTTON *settingsButton;
    BUTTON *exitButton;
};

#endif // WINDOWMANAGER_H
