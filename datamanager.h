#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include "mapdata.h"
#include "userdata.h"
#include "ranking.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    // 用户相关操作
    bool addUser(const QString &id, const QString &password);
    bool removeUser(const QString &id);
    bool checkPassword(const QString &id, const QString &password) const;
    QStringList getAllUserIds() const;

    // 地图相关操作
    void addMap(const MapData& mapData);
    MapData getMap(int id) const;
    bool containsMap(int id) const;

    // 排行榜相关操作
    void updateRanking(int levelId, const QString& userId, int penaltySeconds, int steps);
    QVector<Ranking> getRanking(int levelId) const;
    void clearRanking(int levelId);
    void clearAllRankings();
    
    // 清除所有数据（用户、地图和排行榜）
    void clearAllData();

    // 数据持久化
    bool saveToFile() const;
    bool loadFromFile();

private:
    QMap<QString, UserData> users; // 用户ID -> 用户数据
    QMap<int, MapData> maps;       // 地图ID -> 地图数据
    QMap<int, QVector<Ranking>> rankings;   // 关卡ID -> 排行榜数据
    QString currentUserId;  // 当前登录的用户ID
    const QString filePath = "Data.json";
    static const int MAX_RANKING_ENTRIES = 10;  // 每个关卡最多保存的排行榜记录数
};

#endif // DATAMANAGER_H
