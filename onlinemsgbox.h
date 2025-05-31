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
#include <QPaintEvent>
#include <QPainter>
#include <QMovie>
#include "messagebox.h"
#include "socketmanager.h"
#include "lbutton.h"

class OnlineMsgBox : public QWidget
{
    Q_OBJECT
public:
    enum Mode
    {
        CreateMode, // 创建房间模式
        JoinMode    // 加入房间模式
    };

    explicit OnlineMsgBox(QWidget *parent, SocketManager *manager);
    ~OnlineMsgBox();
    QString getLocalIP();
    void setBackgroundImage(const QString &path);
    void setBackgroundGif(const QString &path);
    void setMode(Mode mode); // 设置对话框模式
    void changeipInputEnabled(bool enabled);

signals:
    void enterLevelMode();     // 进入关卡模式的信号，同时设置在线模式
    void clientConnected();    // 客户端成功连接到服务器
    void clientDisconnected(); // 客户端断开连接

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void updateBackground();
    void handleNavigateRequest(const QString &pageName);

public slots:
    bool startServer();
    bool connectToServer();
    void handleConnectionError(const QString &error);
    void handleClientConnected();
    void handleClientDisconnected();
    void handleNewClientConnected(); // 处理新客户端连接
    void handleRoomLeft();           // 处理对方退出房间
    void handleDuplicateId();        // 处理ID重复

private:
    QLabel *ipLabel;
    QLabel *localIPLabel;
    QLabel *portLabel;
    QLineEdit *ipInput;
    QLineEdit *portInput;
    Lbutton *actionButton;
    Lbutton *cancelButton;
    SocketManager *socketManager;
    bool isConnected;
    QPixmap backgroundImage;
    QMovie *backgroundGif;
    bool usingGif;
    QPixmap currentFrame;
    Mode currentMode;
    MessageBox *msgBox;
};

#endif // ONLINEMSGBOX_H
