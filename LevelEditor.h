#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H
#include "BUTTON.h"
#include <QWidget>
#include <qlineedit.h>

class LevelEditor: public QWidget
{
    Q_OBJECT

public:

    LevelEditor(QWidget *parent = nullptr);
    ~LevelEditor();

    BUTTON *randomButton;
    BUTTON *customButton;
    BUTTON *backButton;

private:

};

#endif // LEVELEDITOR_H
