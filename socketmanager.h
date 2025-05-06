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

class SocketManager : public QObject
{
    Q_OBJECT
public:
    explicit SocketManager(QObject *parent = nullptr);
    ~SocketManager();

    bool StartServer();
    bool StartClient(const QString& serverAddress = "127.0.0.1");
    void SendChatMessage(const QString& message, const QString& sender = "User");
    QJsonObject CreateMsg();

signals:
    void newMessageReceived(const QString& sender, const QString& message);
    void clientConnected();
    void clientDisconnected();
    void connectionError(const QString& error);

private slots:
    void handleNewConnection();
    void handleClientDisconnected();
    void handleReadyRead();
    void handleError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer* server;
    QTcpSocket* clientSocket;
    QList<QTcpSocket*> clientSockets;
    QMap<QTcpSocket*, bool> clientsState;
    QMap<QTcpSocket*, QQueue<QJsonObject>> serverSendMsgList;
    static const int SERVER_PORT = 8888;
    bool isServer;

    void ServerAddSendMsgList(QTcpSocket* client, const QJsonObject& msg);
    bool ServerSendMsg(QTcpSocket* client, const QJsonObject& msg);
    void ServerProcessClientSendMsgList(QTcpSocket* client);
    void ServerProcessClientsSendMsgList();
    QJsonObject ServerRecvMsg(QTcpSocket* client); // Will be part of handleReadyRead

    // Helper for sending JSON
    bool sendJson(QTcpSocket* socket, const QJsonObject& json);
    // Helper for processing received data
    void processReceivedData(QTcpSocket* socket, const QByteArray& data);
};

#endif // SOCKETMANAGER_H
