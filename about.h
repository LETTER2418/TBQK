#ifndef ABOUT_H
#define ABOUT_H
#include <QWidget>
#include "button.h"

class about : public QWidget
{
    Q_OBJECT

public:

    about(QWidget *parent = nullptr);

    ~about();

    button *backButton;
private:

};

#endif // ABOUT_H
