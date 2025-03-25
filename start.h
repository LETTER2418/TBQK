#ifndef START_H
#define START_H
#include "button.h"
#include <QWidget>

class start : public QWidget
{
    Q_OBJECT

public:

    start(QWidget *parent = nullptr);

    ~start();


    button *backButton;
private:

};

#endif // START_H
