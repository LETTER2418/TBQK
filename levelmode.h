#ifndef LEVELMODE_H
#define LEVELMODE_H

#include <QWidget>
#include "lbutton.h"

namespace Ui {
class LevelMode;
}

class LevelMode : public QWidget
{
    Q_OBJECT

public:
    explicit LevelMode(QWidget *parent = nullptr);
    ~LevelMode();
    Lbutton *backButton;
    Lbutton *buttons[12];

private:
};

#endif // LEVELMODE_H
