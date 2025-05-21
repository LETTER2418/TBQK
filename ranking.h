#ifndef RANKING_H
#define RANKING_H

#include <QString>
#include <QJsonObject>

// 排行榜条目类
class Ranking
{
public:
    QString userId;
    int penaltySeconds;
    int steps;

    // 用于 JSON 序列化和反序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
};

#endif // RANKING_H 
