#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <algorithm>

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

// ========== 排行榜操作 ==========
void DataManager::updateRanking(int levelId, const QString& userId, int penaltySeconds)
{
    QVector<Ranking>& levelRanking = rankings[levelId];
    
    // 查找用户是否已经在排行榜中
    auto it = std::find_if(levelRanking.begin(), levelRanking.end(),
                          [&userId](const Ranking& entry) {
                              return entry.userId == userId;
                          });
    
    if (it != levelRanking.end()) {
        // 如果用户已存在且新成绩更好，更新成绩
        if (penaltySeconds < it->penaltySeconds) {
            it->penaltySeconds = penaltySeconds;
        }
    } else {
        // 添加新的排行榜条目
        Ranking newEntry;
        newEntry.userId = userId;
        newEntry.penaltySeconds = penaltySeconds;
        levelRanking.append(newEntry);
    }
    
    // 按照罚时升序排序
    std::sort(levelRanking.begin(), levelRanking.end(),
              [](const Ranking& a, const Ranking& b) {
                  return a.penaltySeconds < b.penaltySeconds;
              });
    
    // 只保留前N名
    if (levelRanking.size() > MAX_RANKING_ENTRIES) {
        levelRanking.resize(MAX_RANKING_ENTRIES);
    }
    
    saveToFile();
}

QVector<Ranking> DataManager::getRanking(int levelId) const
{
    return rankings.value(levelId);
}

void DataManager::clearRanking(int levelId)
{
    if (rankings.contains(levelId)) {
        rankings[levelId].clear();
        saveToFile();
    }
}

void DataManager::clearAllRankings()
{
    rankings.clear();
    saveToFile();
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
    
    // 写入排行榜数据
    QJsonObject rankingsObj;
    for (auto it = rankings.begin(); it != rankings.end(); ++it) {
        QJsonArray levelRankings;
        for (const auto& entry : it.value()) {
            levelRankings.append(entry.toJson());
        }
        rankingsObj[QString::number(it.key())] = levelRankings;
    }
    root["rankings"] = rankingsObj;

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
    
    // 加载排行榜数据
    rankings.clear();
    QJsonObject rankingsObj = root["rankings"].toObject();
    for (const QString& levelId : rankingsObj.keys()) {
        QJsonArray levelRankings = rankingsObj[levelId].toArray();
        QVector<Ranking>& levelRankingVector = rankings[levelId.toInt()];
        
        for (const auto& entryVal : levelRankings) {
            Ranking entry;
            entry.fromJson(entryVal.toObject());
            levelRankingVector.append(entry);
        }
        
        // 确保排序和大小限制
        std::sort(levelRankingVector.begin(), levelRankingVector.end(),
                  [](const Ranking& a, const Ranking& b) {
                      return a.penaltySeconds < b.penaltySeconds;
                  });
        
        if (levelRankingVector.size() > MAX_RANKING_ENTRIES) {
            levelRankingVector.resize(MAX_RANKING_ENTRIES);
        }
    }

    qDebug() << "加载成功：" << filePath;
    return true;
}
