#ifndef ONLINEMODE_H
#define ONLINEMODE_H

#include <QWidget>
#include <QVBoxLayout>
#include "lbutton.h"
#include "onlinemsgbox.h"

class OnlineMode : public QWidget
{
    Q_OBJECT
    
public:
    explicit OnlineMode(QWidget *parent = nullptr);

private slots:
    void onCreateRoom();
    void onJoinRoom();

public:
    OnlineMsgBox *msgBox;

private:
    Lbutton *createRoomButton;
    Lbutton *joinRoomButton;
};

#endif // ONLINEMODE_H
