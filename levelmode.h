#ifndef LEVELMODE_H
#define LEVELMODE_H

#include <QWidget>

namespace Ui {
class LevelMode;
}

class LevelMode : public QWidget
{
    Q_OBJECT

public:
    explicit LevelMode(QWidget *parent = nullptr);
    ~LevelMode();

private:
    Ui::LevelMode *ui;
};

#endif // LEVELMODE_H
