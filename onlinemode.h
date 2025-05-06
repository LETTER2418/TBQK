#ifndef ONLINEMODE_H
#define ONLINEMODE_H

#include <QWidget>
#include <QVBoxLayout>
#include "lbutton.h"

class OnlineMode : public QWidget
{
    Q_OBJECT
    
public:
    explicit OnlineMode(QWidget *parent = nullptr);

private slots:
    void onCreateRoom();
    void onJoinRoom();
    
private:
    Lbutton *createRoomButton;
    Lbutton *joinRoomButton;
};

#endif // ONLINEMODE_H
