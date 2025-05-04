#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "lbutton.h"
#include <QWidget>
#include <qlineedit.h>

class LevelEditor: public QWidget
{
    Q_OBJECT

public:

    LevelEditor(QWidget *parent = nullptr);
    ~LevelEditor();

    Lbutton *randomButton;
    Lbutton *customButton;
    Lbutton *backButton;

private:

};

#endif // LEVELEDITOR_H
