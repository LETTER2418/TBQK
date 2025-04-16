#ifndef RANDOMMAP_H
#define RANDOMMAP_H
#include <QWidget>
#include "BUTTON.h"

class RandomMap: public QWidget
{
    Q_OBJECT

public:

    RandomMap(QWidget *parent = nullptr);
    ~RandomMap();

    BUTTON *backButton;

private:

};

#endif // RANDOMMAP_H
