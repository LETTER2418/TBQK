#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <algorithm>
#include <QFileInfo>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    loadFromFile();
}

// ========== 用户操作 ==========
bool DataManager::addUser(const QString &id, const QString &password)
{
    if (users.contains(id))
        {
            return false;
        }
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
    clearRanking(mapData.id);
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
void DataManager::updateRanking(int levelId, const QString& userId, int penaltySeconds, int steps)
{

    // 先确保从文件加载最新的排行榜数据
    loadFromFile();

    QVector<Ranking>& levelRanking = rankings[levelId];

    // 查找用户是否已经在排行榜中
    auto it = std::find_if(levelRanking.begin(), levelRanking.end(),
                           [&userId](const Ranking & entry)
    {
        return entry.userId == userId;
    });

    if (it != levelRanking.end())
        {
            // 如果用户已存在，分别更新时间和步数为各自的最小值
            if (penaltySeconds < it->penaltySeconds)
                {
                    it->penaltySeconds = penaltySeconds;
                }
            if (steps < it->steps)
                {
                    it->steps = steps;
                }
        }
    else
        {
            //添加新的排行榜条目
            Ranking newEntry;
            newEntry.userId = userId;
            newEntry.penaltySeconds = penaltySeconds;
            newEntry.steps = steps;
            levelRanking.append(newEntry);
        }

    // 按照罚时升序排序，时间相同时按步数升序排序
    std::sort(levelRanking.begin(), levelRanking.end(),
              [](const Ranking & a, const Ranking & b)
    {
        if (a.penaltySeconds != b.penaltySeconds)
            {
                return a.penaltySeconds < b.penaltySeconds;
            }
        return a.steps < b.steps; // 时间相同，比较步数
    });

    // 只保留前N名
    if (levelRanking.size() > MAX_RANKING_ENTRIES)
        {
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
    if (rankings.contains(levelId))
        {
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

    if (!file.open(QIODevice::WriteOnly))
        {
            qWarning() << "无法打开文件：" << filePath;
            return false;
        }

    QJsonObject root;

    // 写入用户数据
    QJsonObject userObj;
    for (auto it = users.begin(); it != users.end(); ++it)
        {
            userObj[it.key()] = it->toJson();
        }
    root["users"] = userObj;

    // 写入地图数据
    QJsonArray mapArray;
    for (const auto& map : maps)
        {
            mapArray.append(map.toJson());
        }
    root["maps"] = mapArray;

    // 写入排行榜数据
    QJsonObject rankingsObj;
    for (auto it = rankings.begin(); it != rankings.end(); ++it)
        {
            QJsonArray levelRankings;
            for (const auto& entry : it.value())
                {
                    levelRankings.append(entry.toJson());
                }
            rankingsObj[QString::number(it.key())] = levelRankings;
        }
    root["rankings"] = rankingsObj;

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool DataManager::loadFromFile()
{
    //防止多次loadFromFile叠加rankings
    clearAllData();

    QFile file(filePath);
    if (!file.exists())
        {
            return false;
        }
    if (!file.open(QIODevice::ReadOnly))
        {
            qWarning() << "无法读取文件：" << filePath;
            return false;
        }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        {
            qWarning() << "JSON 格式错误";
            return false;
        }

    QJsonObject root = doc.object();

    // 加载用户数据
    QJsonObject userObj = root["users"].toObject();
    for (const QString &id : userObj.keys())
        {
            QJsonObject userDataObj = userObj[id].toObject();
            UserData userData;
            userData.fromJson(userDataObj);
            users[id] = userData;
        }

    // 加载地图数据
    QJsonArray mapArray = root["maps"].toArray();
    for (const auto& mapVal : mapArray)
        {
            MapData mapData;
            mapData.fromJson(mapVal.toObject());
            maps.insert(mapData.id, mapData);
        }

    QJsonObject rankingsObj = root["rankings"].toObject();
    for (const QString& levelIdStr : rankingsObj.keys())   // 使用 levelIdStr 避免重定义
        {
            int levelId = levelIdStr.toInt();
            QJsonArray levelRankingsJson = rankingsObj[levelIdStr].toArray(); // 使用 levelIdStr
            QVector<Ranking>& levelRankingVector = rankings[levelId]; // 获取引用 (如果不存在则创建)

            for (const auto& entryVal : levelRankingsJson)   // 使用 levelRankingsJson
                {
                    Ranking entry;
                    entry.fromJson(entryVal.toObject());
                    levelRankingVector.append(entry);
                }
            
            // 确保加载后也排序和限制大小
            std::sort(levelRankingVector.begin(), levelRankingVector.end(),
                      [](const Ranking & a, const Ranking & b)
            {
                if (a.penaltySeconds != b.penaltySeconds)
                    {
                        return a.penaltySeconds < b.penaltySeconds;
                    }
                return a.steps < b.steps;
            });

            if (levelRankingVector.size() > MAX_RANKING_ENTRIES)
                {
                    levelRankingVector.resize(MAX_RANKING_ENTRIES);
                }
        }

    return true;
}

// 清除所有数据（用户、地图和排行榜）
void DataManager::clearAllData()
{
    users.clear();
    maps.clear();
    rankings.clear();
}
