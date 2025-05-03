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
#include "mapmsgbox.h"
#include "mapmanager.h"
#include "savemapmsgbox.h"
#include "savemapmsgbox.h"
#include "levelmode.h"
#include "custommap.h"
#include "mapmsgbox.h"

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
    MapMsgBox *randomMapMsgBox;
    MapMsgBox *customMapMsgBox;
    SaveMapMsgBox   *saveRandomMapMsgBox;
    SaveMapMsgBox   *saveCustomMapMsgBox;

    // Pages
    QWidget     *mainPage;
    Start       *startPage;
    About       *aboutPage;
    Menu        *menuPage;
    LevelEditor *levelEditorPage;
    RandomMap   *randomMapPage;
    LevelMode *levelModePage;
    CustomMap *customMapPage;

    //Manager
    MapManager *mapManager;
};

#endif // MAINWINDOW_H
