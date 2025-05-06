#ifndef ONLINEMSGBOX_H
#define ONLINEMSGBOX_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QMovie>
#include "socketmanager.h"

class OnlineMsgBox : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineMsgBox(QWidget *parent = nullptr);
    ~OnlineMsgBox();
    QString getLocalIP();
    void setBackgroundImage(const QString& path);
    void setBackgroundGif(const QString& path);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateBackground();

public slots:
    void startServer();
    void connectToServer();
    void sendMessage();
    void displayMessage(const QString& sender, const QString& message);
    void handleConnectionError(const QString& error);
    void handleClientConnected();
    void handleClientDisconnected();

private:
    QLabel *ipLabel;
    QLabel *localIPLabel;
    QLabel *portLabel;
    QLineEdit *ipInput;
    QLineEdit *portInput;
    QPushButton *serverButton;
    QPushButton *clientButton;
    QPushButton *sendButton;
    QLineEdit *messageInput;
    QTextEdit *chatDisplay;
    
    SocketManager *socketManager;
    bool isConnected;
    QPixmap backgroundImage;
    QMovie *backgroundGif;
    bool usingGif;
    QPixmap currentFrame;
};

#endif // ONLINEMSGBOX_H
