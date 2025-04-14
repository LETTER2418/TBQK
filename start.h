#ifndef START_H
#define START_H
#include "BUTTON.h"
#include <QWidget>
#include <qlineedit.h>
#include "MessageBox.h"

class start : public QWidget
{
    Q_OBJECT

public:

    start(QWidget *parent = nullptr);
    void onLoginClicked();
    void onRegisterClicked();
    ~start();

    MessageBox *messageBox;
    BUTTON *backButton;
    BUTTON *loginButton;
    BUTTON *registerButton;
    BUTTON *guestButton;
    BUTTON *showPasswordButton;

private:
    QLineEdit *accountLineEdit;
    QLineEdit *passwordLineEdit;
};

#endif // START_H
