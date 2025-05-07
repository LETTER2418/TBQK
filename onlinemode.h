#ifndef ONLINEMODE_H
#define ONLINEMODE_H

#include <QWidget>
#include <QVBoxLayout>
#include "lbutton.h"
#include "onlinemsgbox.h"
#include "socketmanager.h"

class OnlineMode : public QWidget
{
    Q_OBJECT
    
public:
    explicit OnlineMode(QWidget *parent = nullptr, SocketManager* manager = nullptr);

private slots:
    void onCreateRoom();
    void onJoinRoom();

public:
    OnlineMsgBox *msgBox;
    SocketManager *socketManager;

private:
    Lbutton *createRoomButton;
    Lbutton *joinRoomButton;
};

#endif // ONLINEMODE_H
