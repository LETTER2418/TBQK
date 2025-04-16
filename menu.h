#ifndef MENU_H
#define MENU_H
#include "BUTTON.h"
#include <QWidget>
#include <qlineedit.h>

class Menu : public QWidget
{
    Q_OBJECT

public:

    Menu(QWidget *parent = nullptr);
    ~Menu();

    BUTTON *logoutButton;
    BUTTON *levelEditorButton;
    BUTTON *rankButton;
    BUTTON *onlineButton;
    BUTTON *levelModeButton;

private:

};

#endif // MENU_H
