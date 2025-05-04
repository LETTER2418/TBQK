#include "userdata.h"

UserData::UserData(QString password_)
{
    password = password_;
}

QJsonObject UserData::toJson()const
{

    QJsonObject userObj;
    userObj["password"] = password;
    return userObj;
}

void UserData::fromJson(const QJsonObject& obj)
{
    password = obj["password"].toString();
}

QString UserData::getPassword() const
{
    return password;
}
