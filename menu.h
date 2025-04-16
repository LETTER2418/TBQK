#ifndef MENU_H
#define MENU_H
#include "BUTTON.h"
#include <QWidget>
#include <qlineedit.h>

class menu : public QWidget
{
    Q_OBJECT

public:

    menu(QWidget *parent = nullptr);
    ~menu();

    BUTTON *logoutButton;
    BUTTON *levelEditorButton;
    BUTTON *rankButton;
    BUTTON *onlineButton;
    BUTTON *levelModeButton;

private:

};

#endif // MENU_H
