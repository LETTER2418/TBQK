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
#include "savemapmsgbox.h"
#include "savemapmsgbox.h"
#include "levelmode.h"
#include "custommap.h"
#include "mapmsgbox.h"
#include "datamanager.h"
#include "game.h"
#include "rankpage.h"
#include "setting.h"
#include "onlinemode.h"
#include "socketmanager.h"

class MainWindow :public Widget{
    Q_OBJECT

public:
    MainWindow(Widget *parent = nullptr);
    ~MainWindow();

private slots:
    void onClientReceivedGameState(const MapData& mapData);

private:
    QStackedWidget *pageStack;

    //button
    Lbutton* backbutton;
    Lbutton *startButton;
    Lbutton *aboutButton;
    Lbutton *settingButton;
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
    Game *gamePage;
    RankPage *rankPage;
    Setting *settingPage;
    OnlineMode *onlineModePage;
    
    //Manager
    DataManager *dataManager;
    SocketManager* socketManager; // SocketManager 成员指针
};

#endif // MAINWINDOW_H
