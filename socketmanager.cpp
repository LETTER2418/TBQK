#include "socketmanager.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDateTime>
#include <QBuffer>

SocketManager::SocketManager(QObject *parent)
    : QObject(parent), server(nullptr), clientSocket(nullptr), isServer(false), connectionTimer(nullptr)
{
}

SocketManager::~SocketManager()
{
    // 清理资源
    if (server)
    {
        server->close();
        delete server;
    }

    if (clientSocket)
    {
        if (clientSocket->state() == QAbstractSocket::ConnectedState)
        {
            clientSocket->disconnectFromHost();
            if (clientSocket->state() != QAbstractSocket::UnconnectedState)
            {
                clientSocket->waitForDisconnected();
            }
        }
        delete clientSocket;
    }

    // 清理定时器
    if (connectionTimer)
    {
        connectionTimer->stop();
        delete connectionTimer;
    }

    // 清理所有客户端连接（如果是服务器）
    for (QTcpSocket *socket : clientSockets)
    {
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            socket->disconnectFromHost();
            if (socket->state() != QAbstractSocket::UnconnectedState)
            {
                socket->waitForDisconnected();
            }
        }
        delete socket;
    }
    clientSockets.clear();
    clientsState.clear();
    serverSendMsgList.clear();
}

bool SocketManager::StartServer()
{
    // 确保之前的实例已清理
    if (server)
    {
        server->close();
        delete server;
    }

    server = new QTcpServer(this);

    // 连接信号
    connect(server, &QTcpServer::newConnection, this, &SocketManager::handleNewConnection);

    // 尝试在指定端口上启动服务器
    if (!server->listen(QHostAddress::Any, SERVER_PORT))
    {
        emit connectionError(QString("无法启动服务器: %1").arg(server->errorString()));
        return false;
    }

    isServer = true;
    return true;
}

void SocketManager::StartClient(const QString &serverAddress)
{
    // 确保之前的连接已断开
    if (clientSocket)
    {
        if (clientSocket->state() == QAbstractSocket::ConnectedState)
        {
            clientSocket->disconnectFromHost();
            if (clientSocket->state() != QAbstractSocket::UnconnectedState)
            {
                clientSocket->waitForDisconnected();
            }
        }
        delete clientSocket;
    }

    // 清理旧的定时器
    if (connectionTimer)
    {
        connectionTimer->stop();
        connectionTimer->disconnect();
        delete connectionTimer;
        connectionTimer = nullptr;
    }

    // 创建新的定时器，设置为5秒超时
    connectionTimer = new QTimer(this);
    connectionTimer->setSingleShot(true);
    connect(connectionTimer, &QTimer::timeout, this, &SocketManager::handleConnectionTimeout);

    clientSocket = new QTcpSocket(this);

    // 连接信号
    connect(clientSocket, &QTcpSocket::connected, this, [this]()
            {
        // 连接成功，停止定时器
        if (connectionTimer)
            {
                connectionTimer->stop();
            }

        // 连接成功后立即发送自己的ID，用于服务端检查ID是否重复
        if (!localUserId.isEmpty())
            {
                QJsonObject idCheckMsg;
                idCheckMsg["type"] = "id_check";
                idCheckMsg["userId"] = localUserId;
                idCheckMsg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
                sendJson(clientSocket, idCheckMsg);
            }

        emit clientConnected(); });
    connect(clientSocket, &QTcpSocket::disconnected, this, &SocketManager::handleClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &SocketManager::handleReadyRead);
    connect(clientSocket, &QTcpSocket::errorOccurred, this, &SocketManager::handleError);

    // 尝试连接到服务器
    clientSocket->connectToHost(serverAddress, SERVER_PORT);

    // 启动5秒定时器
    connectionTimer->start(5000);

    isServer = false;
}

void SocketManager::SendChatMessage(const QString &message, const QString &sender)
{
    // 创建JSON消息对象
    QJsonObject jsonMessage;
    jsonMessage["type"] = "chat";
    jsonMessage["sender"] = sender;
    jsonMessage["message"] = message;
    jsonMessage["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    if (isServer)
    {
        // 服务器广播消息给所有客户端
        for (QTcpSocket *clientSocket : clientSockets)
        {
            ServerAddSendMsgList(clientSocket, jsonMessage);
        }
        ServerProcessSendClientsMsgList();
    }
    else if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState)
    {
        // 客户端发送消息给服务器
        sendJson(clientSocket, jsonMessage);
    }
}

void SocketManager::SendGameState(const MapData &mapData)
{
    // 创建JSON消息对象
    QJsonObject jsonMessage;
    jsonMessage["type"] = "gameState";
    jsonMessage["mapData"] = mapData.toJson();
    jsonMessage["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    if (isServer)
    {
        // 服务器广播游戏状态给所有客户端
        for (QTcpSocket *clientSocket : clientSockets)
        {
            ServerAddSendMsgList(clientSocket, jsonMessage);
        }
        ServerProcessSendClientsMsgList();
        // qDebug() << "服务端发送地图数据给" << clientSockets.size() << "个client";
    }
    else if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState)
    {
        // 客户端发送游戏状态给服务器
        sendJson(clientSocket, jsonMessage);
        // qDebug() << "客户端发送地图数据";
    }
}

void SocketManager::SendAvatarImage(const QPixmap &avatar, const QString &userId, const QString &type)
{
    // 将QPixmap转换为字节数组
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    avatar.save(&buffer, "PNG");
    buffer.close();

    // 创建JSON消息对象
    QJsonObject jsonMessage;
    jsonMessage["type"] = type;
    jsonMessage["userId"] = userId;
    jsonMessage["avatar_data"] = QString(byteArray.toBase64()); // Base64编码图片数据
    jsonMessage["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    if (isServer)
    {
        // 服务器广播图片数据给所有客户端
        for (QTcpSocket *clientSocket : clientSockets)
        {
            ServerAddSendMsgList(clientSocket, jsonMessage);
        }
        ServerProcessSendClientsMsgList();
    }
    else if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState)
    {
        // 客户端发送图片数据给服务器
        sendJson(clientSocket, jsonMessage);
    }
}

void SocketManager::handleNewConnection()
{
    qDebug() << "收到新的客户端连接";

    // 接收新的客户端连接
    QTcpSocket *clientSocket = server->nextPendingConnection();

    // 连接信号
    connect(clientSocket, &QTcpSocket::disconnected, this, &SocketManager::handleClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &SocketManager::handleReadyRead);
    connect(clientSocket, &QTcpSocket::errorOccurred, this, &SocketManager::handleError);

    // 添加到客户端列表 - 但暂不发送导航消息，等待ID验证后再决定
    clientSockets.append(clientSocket);
    clientsState[clientSocket] = true;                       // 设置初始状态
    serverSendMsgList[clientSocket] = QQueue<QJsonObject>(); // 初始化消息队列
}

void SocketManager::handleClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    if (isServer)
    {
        // qDebug() << "Server handleClientDisconnected";
        // 服务器模式：处理客户端断开连接
        if (socket && clientSockets.contains(socket))
        {
            qDebug() << "断开和客户端连接";
            emit roomLeft();
            // 从客户端列表中移除
            clientSockets.removeOne(socket);
            clientsState.remove(socket);
            serverSendMsgList.remove(socket);

            // 使用deleteLater安全销毁套接字
            socket->deleteLater();
        }
    }
    else
    {
        // 客户端模式：处理与服务器断开连接
        emit clientDisconnected();
    }
}

void SocketManager::handleReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
    {
        return;
    }

    // 读取所有可用数据
    QByteArray data = socket->readAll();
    if (data.isEmpty())
    {
        return;
    }

    // 处理接收到的数据
    processReceivedData(data);
}

void SocketManager::handleConnectionTimeout()
{
    // 处理连接超时
    if (clientSocket && clientSocket->state() != QAbstractSocket::ConnectedState)
    {
        clientSocket->abort(); // 中止连接
        emit connectionError("连接超时，无法连接到服务器");
    }
}

void SocketManager::handleError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QString errorMsg;

    // 如果定时器在运行，停止定时器
    if (connectionTimer && connectionTimer->isActive())
    {
        connectionTimer->stop();
    }

    // 记录详细的错误信息
    qDebug() << "Socket error occurred:" << socketError
             << "Socket state:" << (socket ? socket->state() : QAbstractSocket::UnconnectedState)
             << "Is server:" << isServer;

    // 忽略RemoteHostClosedError，这个会由handleClientDisconnected处理
    if (socketError == QAbstractSocket::RemoteHostClosedError)
    {
        // 不发出错误消息
        return;
    }

    if (socket)
    {
        // 获取错误信息
        errorMsg = socket->errorString();
    }
    else
    {
        // 根据错误类型生成错误信息
        switch (socketError)
        {
        case QAbstractSocket::ConnectionRefusedError:
            errorMsg = "连接被拒绝";
            break;
        case QAbstractSocket::HostNotFoundError:
            errorMsg = "主机未找到";
            break;
        default:
            errorMsg = "发生网络错误";
        }
    }

    emit connectionError(errorMsg);
}

void SocketManager::ServerAddSendMsgList(QTcpSocket *client, const QJsonObject &msg)
{
    if (!client)
    {
        return;
    }

    if (clientsState.contains(client) && clientsState[client] && client->state() == QAbstractSocket::ConnectedState)
    {
        serverSendMsgList[client].enqueue(msg);
    }
}

bool SocketManager::ServerSendMsg(QTcpSocket *client, const QJsonObject &msg)
{
    if (!isSocketValid(client))
    {
        return false;
    }

    try
    {
        // 直接实现发送逻辑，而不是调用返回 void 的 sendJson
        QJsonDocument doc(msg);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        data.append('\n');

        qint64 bytesWritten = client->write(data);
        client->flush();

        return (bytesWritten == data.size());
    }
    catch (...)
    {
        qDebug() << "发送消息时发生异常";
        return false;
    }
}

void SocketManager::ServerProcessSendClientMsgList(QTcpSocket *client)
{
    if (!serverSendMsgList.contains(client))
    {
        return;
    }

    QQueue<QJsonObject> &msgQueue = serverSendMsgList[client];
    while (!msgQueue.isEmpty())
    {
        QJsonObject msg = msgQueue.dequeue();
        ServerSendMsg(client, msg);
    }
}

void SocketManager::ServerProcessSendClientsMsgList()
{
    for (QTcpSocket *client : clientSockets)
    {
        ServerProcessSendClientMsgList(client);
    }
}

bool SocketManager::sendJson(QTcpSocket *socket, const QJsonObject &json)
{
    if (!isSocketValid(socket))
    {
        return false;
    }

    // 将JSON对象转换为文档再转换为字节数组
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // 添加消息结束标记
    data.append('\n');

    // 写入数据
    qint64 totalBytes = data.size();
    qint64 bytesWritten = socket->write(data);
    socket->flush();

    if (bytesWritten != totalBytes)
    {
        qWarning() << "Failed to send complete data. Sent:" << bytesWritten << "Total:" << totalBytes;
        return false;
    }

    return true;
}

void SocketManager::processReceivedData(const QByteArray &data)
{
    static QByteArray buffer; // 静态缓冲区，用于存储不完整的数据
    buffer.append(data);

    // 尝试从缓冲区中提取完整的JSON消息
    while (!buffer.isEmpty())
    {
        // 查找消息结束标记
        int endIndex = buffer.indexOf('\n');
        if (endIndex == -1)
        {
            // 没有找到结束标记，说明数据还不完整
            return;
        }

        // 提取一条完整的消息
        QByteArray jsonData = buffer.left(endIndex).trimmed();
        buffer = buffer.mid(endIndex + 1);

        if (jsonData.isEmpty())
        {
            continue;
        }

        // 如果数据被额外转义，进行处理
        if (jsonData.startsWith('\"') && jsonData.endsWith('\"'))
        {
            jsonData = jsonData.mid(1, jsonData.length() - 2);
            // 替换转义字符
            jsonData.replace("\\\"", "\"");
            jsonData.replace("\\\\", "\\");
            jsonData.replace("\\n", "\n");
            jsonData.replace("\\r", "\r");
            jsonData.replace("\\t", "\t");
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError)
        {
            qWarning() << "JSON parsing error:" << parseError.errorString()
                       << "\nOffset:" << parseError.offset
                       << "\nData:" << jsonData;
            continue;
        }

        if (!doc.isObject())
        {
            qWarning() << "Received non-JSON object data:" << jsonData;
            continue;
        }

        QJsonObject json = doc.object();

        // 验证必要的字段
        if (!json.contains("type"))
        {
            qWarning() << "Missing 'type' field in JSON data";
            continue;
        }

        QString type = json["type"].toString();
        // qDebug() << "Processing message of type:" << type;

        // 处理不同类型的消息
        if (type == "id_check" && isServer)
        {
            QString clientId = json["userId"].toString();
            QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());

            if (senderSocket && clientId == localUserId)
            {
                // 发现ID重复，标记这个套接字，使其断开连接时不触发roomLeft信号
                if (clientSockets.contains(senderSocket))
                {
                    clientsState[senderSocket] = false; // 标记为特殊状态

                    // 发现ID重复
                    // 发送拒绝连接消息
                    SendConnectionRejected(senderSocket, "用户ID重复，无法加入房间");
                    // 确保信号只触发一次
                    static bool emitted = false;
                    if (!emitted)
                    {
                        emit duplicateIdDetected();
                        emitted = true;
                    }

                    // 延迟断开连接，确保消息能被发送
                    QTimer::singleShot(300, this, [this, senderSocket]()
                                       {
                                        if (senderSocket && clientSockets.contains(senderSocket) && senderSocket->state() == QAbstractSocket::ConnectedState)
                                            {
                                                senderSocket->disconnectFromHost();
                                            } });
                }
                return;
            }
            else if (senderSocket)
            {
                // ID检查通过，设置套接字状态并发送欢迎消息和导航指令
                // 发送导航到 levelModePage 的消息
                if (clientSockets.contains(senderSocket))
                {
                    QJsonObject navigateMsg;
                    navigateMsg["type"] = "navigateTo";
                    navigateMsg["page"] = "levelModePage";
                    ServerAddSendMsgList(senderSocket, navigateMsg);

                    // 发送服务器用户ID - 使用空消息，直接发送用户ID
                    if (!localUserId.isEmpty())
                    {
                        QJsonObject userIdMsg;
                        userIdMsg["type"] = "chat";
                        userIdMsg["sender"] = localUserId;
                        userIdMsg["message"] = ""; // 发送空消息，只传递ID
                        userIdMsg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
                        ServerAddSendMsgList(senderSocket, userIdMsg);
                    }

                    // 发送新客户端连接的信号，通知UI
                    emit newClientConnected();
                    ServerProcessSendClientMsgList(senderSocket); // 统一处理消息队列
                }
            }
        }
        else if (type == "connection_rejected" && !isServer)
        {
            // 客户端收到拒绝连接消息
            QString reason = json["reason"].toString();
            emit connectionError(reason);

            // 主动断开连接
            if (clientSocket)
            {
                clientSocket->disconnectFromHost();
            }
        }
        else if (type == "chat")
        {
            if (!json.contains("sender") || !json.contains("message"))
            {
                qWarning() << "Missing required fields in chat message";
                continue;
            }
            QString sender = json["sender"].toString();
            QString message = json["message"].toString();
            emit newMessageReceived(sender, message, false);
        }
        else if (type == "gameState")
        {
            // qDebug() << "收到地图数据";
            QJsonObject mapDataJson = json["mapData"].toObject();
            MapData mapData;
            mapData.fromJson(mapDataJson);
            emit gameStateReceived(mapData);
        }
        else if (type == "navigateTo")
        {
            QString pageName = json["page"].toString();
            if (!pageName.isEmpty())
            {
                emit navigateToPageRequest(pageName);
            }
        }
        else if (type == "avatar_data" || type == "image_data")
        {
            if (!json.contains("userId") || !json.contains("avatar_data"))
            {
                qWarning() << "Missing required fields in image data";
                continue;
            }

            QString userId = json["userId"].toString();
            QString base64Data = json["avatar_data"].toString();

            if (userId.isEmpty() || base64Data.isEmpty())
            {
                qWarning() << "Empty userId or image data";
                continue;
            }

            QByteArray imageData = QByteArray::fromBase64(base64Data.toLatin1());
            if (imageData.isEmpty())
            {
                qWarning() << "Failed to decode base64 image data";
                continue;
            }

            QPixmap image;
            if (!image.loadFromData(imageData))
            {
                qWarning() << "Failed to load image from data";
                continue;
            }

            if (type == "avatar_data")
            {
                emit avatarImageReceived(userId, image);
            }
            else
            {
                emit imageReceived(userId, image, false);
            }
        }
        else if (type == "levelCompleted")
        {
            // 处理对方完成关卡的消息
            int timeUsed = json["timeUsed"].toInt();
            emit levelCompleted(timeUsed);
        }
    }
}

bool SocketManager::isServerMode() const
{
    return isServer;
}

void SocketManager::closeConnection()
{
    if (isServer)
    {
        // 如果是服务器，断开所有客户端连接
        QList<QTcpSocket *> socketsCopy = clientSockets; // 创建副本避免在迭代过程中修改列表
        for (QTcpSocket *socket : socketsCopy)
        {
            if (socket)
            {
                // 安全关闭连接
                if (socket->state() == QAbstractSocket::ConnectedState)
                {
                    socket->disconnectFromHost();
                    if (socket->state() != QAbstractSocket::UnconnectedState)
                    {
                        socket->waitForDisconnected(1000); // 最多等待1秒
                    }
                }

                // 使用deleteLater而不是直接删除
                socket->deleteLater();
            }
        }
        clientSockets.clear();
        clientsState.clear();
        serverSendMsgList.clear();

        // 停止服务器监听
        if (server)
        {
            server->disconnect(); // 断开所有信号连接
            server->close();
            server->deleteLater(); // 使用deleteLater代替直接删除
            server = nullptr;
        }
    }
    else
    {
        // 如果是客户端，断开与服务器的连接
        if (clientSocket)
        {

            // 安全关闭连接
            if (clientSocket->state() == QAbstractSocket::ConnectedState)
            {
                clientSocket->disconnectFromHost();
                if (clientSocket->state() != QAbstractSocket::UnconnectedState)
                {
                    if (!clientSocket->waitForDisconnected(1000))
                    {
                        qDebug() << "强制关闭连接（超时未断开）";
                        clientSocket->abort(); // 强制终止连接
                    }
                }
                else
                {
                    qDebug() << "正常断开连接";
                }
            }

            // 使用deleteLater而不是直接删除
            clientSocket->deleteLater();
            clientSocket = nullptr;
        }
    }

    // 重置状态
    isServer = false;
}

void SocketManager::setLocalUserId(const QString &userId)
{
    localUserId = userId;
}

QString SocketManager::getLocalUserId() const
{
    return localUserId;
}

// 检查套接字是否有效
bool SocketManager::isSocketValid(QTcpSocket *socket) const
{
    // 检查套接字指针是否为空
    if (!socket)
    {
        return false;
    }

    // 检查套接字是否有效和打开
    if (socket->state() == QAbstractSocket::UnconnectedState ||
        !socket->isValid())
    {
        return false;
    }

    return true;
}

// 添加发送拒绝连接消息的方法
void SocketManager::SendConnectionRejected(QTcpSocket *client, const QString &reason)
{
    if (!client || client->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    try
    {
        QJsonObject jsonMessage;
        jsonMessage["type"] = "connection_rejected";
        jsonMessage["reason"] = reason;
        jsonMessage["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

        QJsonDocument doc(jsonMessage);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        data.append('\n');

        client->write(data);
        client->flush();
    }
    catch (...)
    {
        qDebug() << "发送拒绝连接消息时发生异常";
    }
}
