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
#include "lbutton.h"

class OnlineMsgBox : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        CreateMode,  // 创建房间模式
        JoinMode     // 加入房间模式
    };

    explicit OnlineMsgBox(QWidget *parent = nullptr, SocketManager* manager = nullptr);
    ~OnlineMsgBox();
    QString getLocalIP();
    void setBackgroundImage(const QString& path);
    void setBackgroundGif(const QString& path);
    void setMode(Mode mode);  // 设置对话框模式

signals:
    void enterLevelMode();     // 进入关卡模式的信号

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateBackground();
    void handleNavigateRequest(const QString& pageName);

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
    Lbutton *actionButton;
    Lbutton *cancelButton;
    QLineEdit *messageInput;
    QTextEdit *chatDisplay;
    
    SocketManager *socketManager;
    bool isConnected;
    QPixmap backgroundImage;
    QMovie *backgroundGif;
    bool usingGif;
    QPixmap currentFrame;
    Mode currentMode;         // 当前模式
};

#endif // ONLINEMSGBOX_H
