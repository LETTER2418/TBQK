#ifndef ABOUT_H
#define ABOUT_H
#include <QWidget>
#include "BUTTON.h"

class About : public QWidget
{
    Q_OBJECT

public:

    About(QWidget *parent = nullptr);

    ~About();

    BUTTON *backButton;
private:

};

#endif // ABOUT_H
