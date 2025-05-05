#ifndef MAPDATA_H
#define MAPDATA_H

#include "hexcell.h"

class MapData
{
public:
    MapData();
    ~MapData();

    // 将 MapData 转换为 QJsonObject
    QJsonObject toJson() const ;

    // 从 QJsonObject 加载 MapData
    void fromJson(const QJsonObject& obj) ;

    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440); // 地图中心坐标
    QColor color1;
    QColor color2;
    QColor color3;
    int id; // 关卡索引
    QVector<QPoint>path;
    QMap<int,int>penaltyMap;//在to/fromJson，其他类的函数待实现
    QString generation; // "custom" 或 "random"

};

#endif
