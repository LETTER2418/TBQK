#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

struct UserData {
    QString password;
    int level;
    QString rank;
};

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = nullptr);

    bool addUser(const QString &id, const QString &password, int level = 0, const QString &rank = "Bronze");
    bool removeUser(const QString &id);
    bool checkPassword(const QString &id, const QString &password) const;

    bool updateLevel(const QString &id, int level);
    bool updateRank(const QString &id, const QString &rank);

    UserData getUserData(const QString &id) const;
    QStringList getAllUserIds() const;

    bool saveToFile() const;
    bool loadFromFile();

private:
    QMap<QString, UserData> userMap; // 用户ID -> 用户数据
    const QString filePath = "UserInfo.json";
};

extern UserManager userManager;

#endif // USERMANAGER_H
