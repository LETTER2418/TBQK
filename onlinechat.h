#ifndef ONLINECHAT_H
#define ONLINECHAT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QPixmap>
#include <QMap>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "socketmanager.h"
#include "lbutton.h"
#include "datamanager.h"

// 聊天气泡组件
class ChatBubble : public QFrame
{
    Q_OBJECT
public:
    // 构造函数
    ChatBubble(const QString &text, bool isSelf, QPixmap avatar, QWidget *parent = nullptr);
    
    // 更新头像
    void updateAvatar(const QPixmap& newAvatar);
    
    // 获取是否为自己的消息
    bool isSelfMessage() const { return isSelf; }
    
    // 获取关联的用户ID
    QString getUserId() const { return userId; }
    
    // 设置关联的用户ID
    void setUserId(const QString& id) { userId = id; }
    
    // 静态方法：根据文本内容和最大宽度插入换行符
    static QString insertLineBreaks(const QString &text, int maxWidth, const QFont &font);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    QLabel *messageLabel;
    QLabel *avatarLabel;
    bool isSelf;
    QPixmap avatar;
    QString userId; // 存储关联的用户ID
};

class OnlineChat : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineChat(SocketManager* manager, DataManager* dm, QWidget *parent = nullptr);
    ~OnlineChat();
    
    // 发送当前用户头像
    void sendCurrentUserAvatar();
    
    // 清空聊天历史记录
    void clearChatHistory();
    
    // 通知用户存在
    void announceUserPresence();

public slots:
    void displayMessage(const QString& userId, const QString& message, bool isSelfMessage = false);
    
    // 加载用户自定义头像
    void setUserAvatar(const QString& userId);

protected:
    void paintEvent(QPaintEvent *event) override;
    
    // 重写showEvent，在窗口显示时发送用户信息
    void showEvent(QShowEvent *event) override;

private slots:
    void sendMessage();
    // 接收头像图片
    void onAvatarImageReceived(const QString& userId, const QPixmap& avatar);
    
private:
    // 添加水平分隔线
    QFrame* createHLine();
    // 加载用户头像
    QPixmap loadAvatar(const QString& userId);
    // 加载头像配置文件
    QPixmap loadAvatarFromSettings();
    
    SocketManager* socketManager;
    DataManager* dataManager;    // 添加对DataManager的引用
    
    // 顶部用户信息区域
    QWidget* topPanel;
    QLabel* remoteUserLabel;
    
    // 中间聊天内容区域
    QScrollArea* chatScrollArea;
    QWidget* chatContentWidget;
    QVBoxLayout* chatContentLayout;
    
    // 底部输入区域
    QWidget* bottomPanel;
    QTextEdit* messageInput;
    Lbutton* sendButton;
    QVBoxLayout* mainLayout;
    QHBoxLayout* buttonLayout;
    
    // 存储用户头像
    QMap<QString, QPixmap> userAvatars;
    QPixmap defaultAvatar;
    const QString avatarPath = ":/avatar/"; // 头像路径
};

#endif // ONLINECHAT_H