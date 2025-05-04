#ifndef USERDATA_H
#define USERDATA_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QString>
#include <QMap>

class UserData
{
public:
    UserData(QString password_="1");
    // 将 MapData 转换为 QJsonObject
    QJsonObject toJson() const ;

    // 从 QJsonObject 加载 MapData
    void fromJson(const QJsonObject& obj) ;
    QString getPassword();
private:
    QString password;
};

#endif // USERDATA_H
