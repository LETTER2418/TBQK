#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include "mapdata.h"
#include "userdata.h"

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

    // 数据持久化
    bool saveToFile() const;
    bool loadFromFile();

private:
    QMap<QString, UserData> users; // 用户ID -> 用户数据
    QMap<int, MapData> maps;       // 地图ID -> 地图数据
    const QString filePath = "Data.json";
};

extern DataManager dataManager;

#endif // DATAMANAGER_H
