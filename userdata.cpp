#include "userdata.h"

UserData::UserData(QString password_)
{
    password = password_;
    settings = QJsonObject(); // 初始化settings为空对象
}

QJsonObject UserData::toJson() const
{
    QJsonObject userObj;
    userObj["password"] = password;
    userObj["settings"] = settings; // 添加settings字段
    return userObj;
}

void UserData::fromJson(const QJsonObject &obj)
{
    password = obj["password"].toString();

    // 读取settings字段
    if (obj.contains("settings") && obj["settings"].isObject())
        {
            settings = obj["settings"].toObject();
        }
    else
        {
            settings = QJsonObject(); // 如果不存在则初始化为空对象
        }
}

QString UserData::getPassword() const
{
    return password;
}
