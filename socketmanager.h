#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QQueue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QPixmap>
#include "mapdata.h"

class SocketManager : public QObject
{
    Q_OBJECT
public:
    explicit SocketManager(QObject *parent = nullptr);
    ~SocketManager();
    bool StartServer();
    void StartClient(const QString &serverAddress = "127.0.0.1");
    void SendChatMessage(const QString &message, const QString &sender = "User");
    void SendGameState(const MapData &mapData);                         // 发送游戏状态
    void SendAvatarImage(const QPixmap &avatar, const QString &userId); // 发送头像图像数据
    void closeConnection();                                             // 关闭连接
    bool isServerMode() const;                                          // 用于检查当前是否为服务器模式
    void setLocalUserId(const QString &userId);
    QString getLocalUserId() const;
    QList<QTcpSocket *> getClientSockets() const { return clientSockets; }  // 获取客户端连接列表
    QTcpSocket *getClientSocket() const { return clientSocket; }            // 获取客户端套接字（客户端模式）
    bool sendJson(QTcpSocket *socket, const QJsonObject &json);             // 发送JSON数据
    void SendConnectionRejected(QTcpSocket *client, const QString &reason); // 向指定客户端发送拒绝连接消息

signals:
    void newMessageReceived(const QString &sender, const QString &message, bool isSelfMessage);
    void clientConnected();
    void clientDisconnected();
    void connectionError(const QString &error);
    void gameStateReceived(const MapData &mapData);                         // 接收游戏状态的信号
    void navigateToPageRequest(const QString &pageName);                    // 请求页面导航
    void roomLeft();                                                        // 对方退出房间的信号
    void avatarImageReceived(const QString &userId, const QPixmap &avatar); // 接收到头像图像的信号
    void newClientConnected();                                              // 新的客户端连接到服务器的信号
    void duplicateIdDetected();                                             // 检测到重复ID的信号
public slots:
    void handleClientDisconnected();

private slots:
    void handleNewConnection();
    void handleReadyRead();
    void handleError(QAbstractSocket::SocketError socketError);
    void handleConnectionTimeout();

private:
    QTcpServer *server;
    QTcpSocket *clientSocket;
    QList<QTcpSocket *> clientSockets;
    QMap<QTcpSocket *, bool> clientsState;
    QMap<QTcpSocket *, QQueue<QJsonObject>> serverSendMsgList;
    static const int SERVER_PORT = 8888;
    bool isServer;
    QTimer *connectionTimer;
    QString localUserId;

    void ServerAddSendMsgList(QTcpSocket *client, const QJsonObject &msg);
    bool ServerSendMsg(QTcpSocket *client, const QJsonObject &msg);
    void ServerProcessSendClientMsgList(QTcpSocket *client);
    void ServerProcessSendClientsMsgList();
    void processReceivedData(const QByteArray &data);
    bool isSocketValid(QTcpSocket *socket) const; // 检查套接字是否有效
};

#endif // SOCKETMANAGER_H
