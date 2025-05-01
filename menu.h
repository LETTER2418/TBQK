#ifndef MENU_H
#define MENU_H
#include "lbutton.h"
#include <QWidget>
#include <qlineedit.h>

class Menu : public QWidget
{
    Q_OBJECT

public:

    Menu(QWidget *parent = nullptr);
    ~Menu();

    Lbutton *logoutButton;
    Lbutton *levelEditorButton;
    Lbutton *rankButton;
    Lbutton *onlineButton;
    Lbutton *levelModeButton;

private:

};

#endif // MENU_H
