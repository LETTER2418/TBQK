#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QWidget>
#include "mapdata.h"

class MapManager : public QObject
{
    Q_OBJECT

public:
    MapManager(QObject *parent);
    bool saveMap();

    bool loadMap() ;

    void addMap(const MapData& mapData);

    MapData getMap(int id) const ;
    bool containsMap(int id) const ;

private:
    const QString filePath = "MapInfo.json"; // 固定文件路径
    QMap<int, MapData> map; // 地图ID -> 地图数据
};

#endif // MAPMANAGER_H
