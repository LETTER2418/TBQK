#ifndef ABOUT_H
#define ABOUT_H
#include <QWidget>
#include "Lbutton.h"

class About : public QWidget
{
    Q_OBJECT

public:

    About(QWidget *parent = nullptr);

    ~About();

    Lbutton *backButton;
private:

};

#endif // ABOUT_H
