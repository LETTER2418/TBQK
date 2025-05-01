#include "mapmanager.h"
#include <QDebug>

MapManager::MapManager(QObject *parent) : QObject(parent)
{

}

bool MapManager::saveMap() {
    QJsonObject root;
    QJsonArray mapsArray;

    for (const auto& mapData : map) {
        mapsArray.append(mapData.toJson());
    }

    root["maps"] = mapsArray;

    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

bool MapManager::loadMap() {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray mapsArray = root["maps"].toArray();

    map.clear();
    for (const auto& mapVal : mapsArray) {
        MapData mapData;
        mapData.fromJson(mapVal.toObject());
        map.insert(mapData.id, mapData);
    }

    return true;
}

void MapManager::addMap(const MapData& mapData) {
    map.insert(mapData.id, mapData);
}

MapData MapManager::getMap(int id) const {
    return map.value(id);
}

bool MapManager::containsMap(int id) const {
    return map.contains(id);
}
