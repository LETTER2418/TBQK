#ifndef START_H
#define START_H
#include "button.h"
#include <QWidget>
#include <qlineedit.h>

class start : public QWidget
{
    Q_OBJECT

public:

    start(QWidget *parent = nullptr);
    void onLoginClicked();
    void onRegisterClicked();
    ~start();


    button *backButton;
private:
    button *loginButton;
    button *registerButton;
    button *guestButton;
    QLineEdit *accountLineEdit;
    QLineEdit *passwordLineEdit;
};

#endif // START_H
