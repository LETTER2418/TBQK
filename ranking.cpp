#include "ranking.h"
#include <QJsonObject>

QJsonObject Ranking::toJson() const
{
    QJsonObject obj;
    obj["userId"] = userId;
    obj["penaltySeconds"] = penaltySeconds;
    return obj;
}

void Ranking::fromJson(const QJsonObject& obj)
{
    userId = obj["userId"].toString();
    penaltySeconds = obj["penaltySeconds"].toInt();
} 