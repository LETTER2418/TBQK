#ifndef START_H
#define START_H
#include "BUTTON.h"
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
