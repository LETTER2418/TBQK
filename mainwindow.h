#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QVBoxLayout>
#include "widget.h"
#include "lbutton.h"
#include "start.h"
#include "about.h"
#include "menu.h"
#include "leveleditor.h"
#include "randommap.h"
#include "randommapmsgbox.h"
#include "mapmanager.h"
#include "savemapmsgbox.h"
#include "savemapmsgbox.h"
#include "ui_savemapmsgbox.h"

class MainWindow :public Widget{
    Q_OBJECT

public:
    MainWindow(Widget *parent = nullptr);
    ~MainWindow();

private:

    QStackedWidget *pageStack;

    //button
    Lbutton* backbutton;
    Lbutton *startButton;
    Lbutton *aboutButton;
    Lbutton *settingsButton;
    Lbutton *exitButton;

    // MsgBox
    RandomMapMsgBox *randomMapMsgBox;
    SaveMapMsgBox   *saveMapMsgBox;

    // Pages
    QWidget     *mainPage;
    Start       *startPage;
    About       *aboutPage;
    Menu        *menuPage;
    LevelEditor *levelEditorPage;
    RandomMap   *randomMapPage;

    //Manager
    MapManager *mapManager;
};

#endif // MAINWINDOW_H
