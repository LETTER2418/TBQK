#include<hexcell.h>

QJsonObject HexCell::toJson() const
{
    QJsonObject obj;
    obj["center_x"] = center.x();  // 存储中心点 x 坐标
    obj["center_y"] = center.y();  // 存储中心点 y 坐标
    obj["color"] = color.name(QColor::HexArgb);  // 存储颜色（含 Alpha 通道）
    return obj;
}

void HexCell::fromJson(const QJsonObject &obj)
{
    // 恢复 center
    double x = obj["center_x"].toDouble();
    double y = obj["center_y"].toDouble();
    center = QPointF(x, y);

    // 恢复 color
    color = QColor(obj["color"].toString());
}
