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
#include <QMenu>
#include <QGridLayout>
#include <QWidgetAction>
#include <QToolButton>
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
    void updateAvatar(const QPixmap &newAvatar);

    // 获取是否为自己的消息
    bool isSelfMessage() const { return isSelf; }

    // 获取关联的用户ID
    QString getUserId() const { return userId; }

    // 设置关联的用户ID
    void setUserId(const QString &id) { userId = id; }

    // 静态方法：根据文本内容和最大宽度插入换行符
    static QString insertLineBreaks(const QString &text, int maxWidth, const QFont &font);

    // 添加设置时间戳的方法
    void setTimestamp(const QDateTime &timestamp);

    // 获取消息时间戳
    QDateTime getTimestamp() const { return timestamp; }

    // 设置时间标签可见性
    void setTimeVisible(bool visible);

    QString insertZeroWidthSpace(const QString &text, int chunkSize);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *messageLabel;
    QLabel *avatarLabel;
    QLabel *timeLabel; // 添加时间标签
    bool isSelf;
    QPixmap avatar;
    QString userId;      // 存储关联的用户ID
    QDateTime timestamp; // 消息时间戳
};

class OnlineChat : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineChat(SocketManager *manager, DataManager *dm, QWidget *parent = nullptr);
    ~OnlineChat();

    // 发送当前用户头像
    void sendCurrentUserAvatar();

    // 清空聊天历史记录
    void clearChatHistory();

    // 通知用户存在
    void announceUserPresence();

    void displayImage(const QString &userId, const QPixmap &image, bool isSelfImage); // 新增

public slots:
    void displayMessage(const QString &userId, const QString &message, bool isSelfMessage = false);

    // 加载用户自定义头像
    void setUserAvatar(const QString &userId);

protected:
    void paintEvent(QPaintEvent *event) override;

    // 重写showEvent，在窗口显示时发送用户信息
    void showEvent(QShowEvent *event) override;

    // 添加事件过滤器处理Enter键发送
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void sendMessage();
    void sendImage();
    // 接收头像图片
    void onAvatarImageReceived(const QString &userId, const QPixmap &avatar);
    void showEmojiMenu();                   // 显示emoji菜单
    void insertEmoji(const QString &emoji); // 插入emoji到输入框

private:
    // 添加水平分隔线
    QFrame *createHLine();
    // 加载用户头像
    QPixmap loadAvatar(const QString &userId);
    // 加载头像配置文件
    QPixmap loadAvatarFromSettings();

    SocketManager *socketManager;
    DataManager *dataManager; // 添加对DataManager的引用

    // 顶部用户信息区域
    QWidget *topPanel;
    QLabel *remoteUserLabel;

    // 中间聊天内容区域
    QScrollArea *chatScrollArea;
    QWidget *chatContentWidget;
    QVBoxLayout *chatContentLayout;

    // 底部输入区域
    QWidget *bottomPanel;
    QTextEdit *messageInput;
    Lbutton *sendButton;
    QToolButton *imageButton;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;

    // 存储用户头像
    QMap<QString, QPixmap> userAvatars;
    QPixmap defaultAvatar;
    const QString avatarPath = ":/avatar/"; // 头像路径

    // Emoji相关成员
    QToolButton *emojiButton;
    QMenu *emojiMenu;
    void initEmojiMenu();     // 初始化emoji菜单
    QStringList commonEmojis; // 常用emoji列表
};

#endif // ONLINECHAT_H
