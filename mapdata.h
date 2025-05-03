#include "hexcell.h"

class MapData {
public:
    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440); // 地图中心坐标
    QColor color1;
    QColor color2;
    QColor color3;
    int id; // 关卡索引
    QVector<QPoint>path;

    // 将 MapData 转换为 QJsonObject
    QJsonObject toJson() const ;

    // 从 QJsonObject 加载 MapData
    void fromJson(const QJsonObject& obj) ;
};
