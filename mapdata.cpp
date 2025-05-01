#include "mapdata.h"

QJsonObject MapData::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["radius"] = radius;
    obj["center_x"] = center.x();
    obj["center_y"] = center.y();
    obj["color1"] = color1.name();
    obj["color2"] = color2.name();

    QJsonArray hexArray;
    for (const auto& hex : hexagons) {
        hexArray.append(hex.toJson());
    }
    obj["hexagons"] = hexArray;

    return obj;
}

void MapData::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt();
    radius = obj["radius"].toInt();
    center.setX(obj["center_x"].toDouble());
    center.setY(obj["center_y"].toDouble());
    color1 = QColor(obj["color1"].toString());
    color2 = QColor(obj["color2"].toString());

    hexagons.clear();
    QJsonArray hexArray = obj["hexagons"].toArray();
    for (const auto& hexVal : hexArray) {
        HexCell hex;
        QJsonObject hexObj = hexVal.toObject();
        hex.fromJson(hexObj);
        hexagons.append(hex);
    }
}
