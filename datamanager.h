#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QJsonObject>
#include <QPixmap>
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

    // 获取当前用户ID
    QString getCurrentUserId() const
    {
        return currentUserId;
    }
    void setCurrentUserId(const QString &userId)
    {
        currentUserId = userId;
    }

    // 用户设置相关操作
    QJsonObject getUserSettings(const QString &userId) const;
    bool updateUserSettings(const QString &userId, const QJsonObject &settings);

    // 头像路径相关操作
    QString getAvatarPath() const
    {
        return tempAvatarPath;
    }
    void setAvatarPath(const QString &path)
    {
        tempAvatarPath = path;
    }

    // 头像文件相关操作
    bool saveAvatarFile(const QString &userId, const QPixmap &avatar);
    QPixmap loadAvatarFile(const QString &userId) const;
    QString getAvatarDir() const
    {
        return "avatars";    // 头像存储目录
    }

private:
    QMap<QString, UserData> users; // 用户ID -> 用户数据
    QMap<int, MapData> maps;       // 地图ID -> 地图数据
    QMap<int, QVector<Ranking>> rankings;   // 关卡ID -> 排行榜数据
    QString currentUserId;  // 当前登录的用户ID
    QString tempAvatarPath; // 临时存储的头像路径
    const QString filePath = "Data.json";
    static const int MAX_RANKING_ENTRIES = 10;  // 每个关卡最多保存的排行榜记录数
};

#endif // DATAMANAGER_H
