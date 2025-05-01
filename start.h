#ifndef START_H
#define START_H
#include "lbutton.h"
#include <QWidget>
#include <qlineedit.h>
#include "messagebox.h"

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
