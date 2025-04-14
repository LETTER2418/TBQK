#ifndef ABOUT_H
#define ABOUT_H
#include <QWidget>
#include "BUTTON.h"

class about : public QWidget
{
    Q_OBJECT

public:

    about(QWidget *parent = nullptr);

    ~about();

    BUTTON *backButton;
private:

};

#endif // ABOUT_H
