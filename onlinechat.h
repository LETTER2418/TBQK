#ifndef ONLINECHAT_H
#define ONLINECHAT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "socketmanager.h"
#include "lbutton.h"

class OnlineChat : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineChat(SocketManager* sm, QWidget *parent = nullptr);
    ~OnlineChat();

public slots:
    void displayMessage(const QString& sender, const QString& message);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void sendMessage();

private:
    SocketManager* socketManager;  
    QTextEdit* chatDisplay;
    QLineEdit* messageInput;
    Lbutton* sendButton;
    QVBoxLayout* mainLayout;
    QHBoxLayout* inputLayout;
    QPixmap backgroundImage;
    const QString backgroundImagePath=":\\image\\msg.png"; 
};

#endif // ONLINECHAT_H
