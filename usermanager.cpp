#include "usermanager.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>

UserManager::UserManager(QObject *parent) : QObject(parent)
{
}

bool UserManager::addUser(const QString &id, const QString &password, int level, const QString &rank)
{
    if (userMap.contains(id)) return false;
    userMap[id] = { password, level, rank };
    return true;
}

bool UserManager::removeUser(const QString &id)
{
    return userMap.remove(id) > 0;
}

bool UserManager::checkPassword(const QString &id, const QString &password) const
{
    return userMap.contains(id) && userMap[id].password == password;
}

bool UserManager::updateLevel(const QString &id, int level)
{
    if (!userMap.contains(id)) return false;
    userMap[id].level = level;
    return true;
}

bool UserManager::updateRank(const QString &id, const QString &rank)
{
    if (!userMap.contains(id)) return false;
    userMap[id].rank = rank;
    return true;
}

UserData UserManager::getUserData(const QString &id) const
{
    return userMap.value(id, { "", 0, "" });
}

QStringList UserManager::getAllUserIds() const
{
    return userMap.keys();
}

bool UserManager::saveToFile() const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件：" << filePath;
        return false;
    }

    QJsonObject root;
    for (auto it = userMap.begin(); it != userMap.end(); ++it) {
        QJsonObject userObj;
        userObj["password"] = it->password;
        userObj["level"] = it->level;
        userObj["rank"] = it->rank;
        root[it.key()] = userObj;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    qDebug() << "保存成功：" << filePath;
    return true;
}

bool UserManager::loadFromFile()
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
    userMap.clear();

    for (const QString &id : root.keys()) {
        QJsonObject userObj = root[id].toObject();
        UserData userData;
        userData.password = userObj["password"].toString();
        userData.level = userObj["level"].toInt();
        userData.rank = userObj["rank"].toString();
        userMap[id] = userData;
    }

    qDebug() << "加载成功：" << filePath;
    return true;
}
