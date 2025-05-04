#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonValue>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    loadFromFile();
}

// ========== 用户操作 ==========
bool DataManager::addUser(const QString &id, const QString &password)
{
    if (users.contains(id)) return false;
    users[id] = { password };
    return true;
}

bool DataManager::removeUser(const QString &id)
{
    return users.remove(id) > 0;
}

bool DataManager::checkPassword(const QString &id, const QString &password) const
{
    return users.contains(id) && users[id].getPassword() == password;
}

// ========== 地图操作 ==========
void DataManager::addMap(const MapData &mapData)
{
        maps.insert(mapData.id, mapData);
}

MapData DataManager::getMap(int id) const
{
    return maps.value(id);
}

bool DataManager::containsMap(int id) const
{
    return maps.contains(id);
}

QStringList DataManager::getAllUserIds() const
{
    return users.keys();
}

// ========== 数据持久化 ==========
bool DataManager::saveToFile() const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件：" << filePath;
        return false;
    }

    QJsonObject root;

    // 写入用户数据
    QJsonObject userObj;
    for (auto it = users.begin(); it != users.end(); ++it) {
        userObj[it.key()] = it->toJson();
    }
    root["users"] = userObj;

    // 写入地图数据
    QJsonArray mapArray;
    for (const auto& map : maps) {
        mapArray.append(map.toJson());
    }
    root["maps"] = mapArray;

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    qDebug() << "保存成功到：" << filePath;
    return true;
}

bool DataManager::loadFromFile()
{
    QFile file(filePath);
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法读取文件：" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "JSON 格式错误";
        return false;
    }

    QJsonObject root = doc.object();

    // 加载用户数据
    QJsonObject userObj = root["users"].toObject();
    for (const QString &id : userObj.keys()) {
        QJsonObject userDataObj = userObj[id].toObject();
        UserData userData;
        userData.fromJson(userDataObj);
        users[id] = userData;
    }

    // 加载地图数据
    QJsonArray mapArray = root["maps"].toArray();
    for (const auto& mapVal : mapArray) {
        MapData mapData;
        mapData.fromJson(mapVal.toObject());
        maps.insert(mapData.id, mapData);
    }

    qDebug() << "加载成功：" << filePath;
    return true;
}
