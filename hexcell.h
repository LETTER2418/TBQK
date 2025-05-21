#ifndef HEXCELL_H
#define HEXCELL_H

#include<QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class HexCell
{
public:
    QPointF center;
    QColor color;

    QJsonObject toJson() const ;
    void fromJson(const QJsonObject &obj);
};

#endif // HEXCELL_H
