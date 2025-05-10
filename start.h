#ifndef START_H
#define START_H

#include "lbutton.h"
#include <QWidget>
#include <qlineedit.h>
#include "messagebox.h"
#include "datamanager.h"
#include "socketmanager.h"

class Start : public QWidget
{
    Q_OBJECT

public:

    Start(QWidget*, DataManager*, SocketManager*);
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

    QString currentUserId;;

private:
    QLineEdit *accountLineEdit;
    QLineEdit *passwordLineEdit;
    DataManager *dataManager;
    SocketManager *socketManager;
};

#endif // START_H
