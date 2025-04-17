#ifndef START_H
#define START_H
#include "Lbutton.h"
#include <QWidget>
#include <qlineedit.h>
#include "MessageBox.h"

class Start : public QWidget
{
    Q_OBJECT

public:

    Start(QWidget *parent = nullptr);
    void onLoginClicked();
    void onRegisterClicked();
    ~Start();

    MessageBox *YESmessageBox;
    MessageBox *NOmessageBox;
    Lbutton *backButton;
    Lbutton *loginButton;
    Lbutton *registerButton;
    Lbutton *guestButton;
    Lbutton *showPasswordButton;

private:
    QLineEdit *accountLineEdit;
    QLineEdit *passwordLineEdit;
};

#endif // START_H
