#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QListWidget>
#include <QComboBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QToolButton>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QEvent>
#include <QFileDialog>
#include <QTransform>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QGroupBox>
#include <QFileInfo>
#include <QTime>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QPainter>
#include <QBrush>
#include <QEasingCurve>
#include <QPainterPath>
#include "lbutton.h"
#include "messagebox.h"
#include "datamanager.h"

class Setting : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double avatarRotationAngle READ getAvatarRotationAngle WRITE setAvatarRotationAngle)

public:
    explicit Setting(QWidget *parent = nullptr, DataManager *dataManager_ = nullptr);
    ~Setting();

    Lbutton *backButton; // 返回按钮
    // Lbutton *clearDataButton; // 清除数据按钮

    // 音乐播放器相关方法
    void playMusic();
    void pauseMusic();
    void setVolume(int volume);
    void setButtonVolume(int volume);
    void nextSong();
    void previousSong();
    void loadPlaylist();
    void updatePlaylistDisplay();
    void savePlaylistToFile();                       // 保存歌单到本地
    void addSongToPlaylist(const QString &filePath); // 添加歌曲到歌单
    void removeSongFromPlaylist();                   // 从歌单中删除歌曲

    // 头像相关方法
    void loadAvatar(); // 加载头像

    // 事件过滤器
    bool eventFilter(QObject *watched, QEvent *event) override;

    double getAvatarRotationAngle() const;
    void setAvatarRotationAngle(double angle);

protected:
    void showEvent(QShowEvent *event) override;

private:
    DataManager *dataManager;
    MessageBox *confirmMessageBox; // 确认对话框
    MessageBox *successMessageBox; // 成功提示对话框

    // 音乐播放器相关控件和变量
    QMediaPlayer *musicPlayer;      // 音乐播放器
    QAudioOutput *musicAudioOutput; // 音频输出
    QSlider *volumeSlider;          // 音量滑块
    QSlider *buttonVolumeSlider;    // 按钮音效音量滑块
    QToolButton *playButton;        // 播放按钮
    QToolButton *pauseButton;       // 暂停按钮
    QToolButton *nextButton;        // 下一首按钮
    QToolButton *prevButton;        // 上一首按钮
    QSlider *progressSlider;        // 进度条
    QLabel *timeLabel;              // 时间标签
    Lbutton *openFileButton;        // 打开文件按钮
    Lbutton *removeSongButton;      // 删除歌曲按钮
    QLabel *currentSongLabel;       // 当前歌曲标签
    QComboBox *playModeComboBox;    // 播放模式选择框
    QListWidget *playlistWidget;    // 歌单显示列表

    // 头像相关控件和变量
    QLabel *avatarLabel;         // 头像显示标签
    Lbutton *uploadAvatarButton; // 上传头像按钮
    QString avatarPath;          // 头像文件路径

    QStringList playlist; // 播放列表
    int currentIndex;     // 当前播放索引
    enum PlayMode
    {
        SingleLoop,     // 单曲循环
        SequentialPlay, // 顺序播放
        RandomPlay      // 随机播放
    };
    PlayMode currentPlayMode;                         // 当前播放模式
    const QString playlistFilePath = "playlist.json"; // 歌单保存路径

    // 头像旋转动画相关
    QPropertyAnimation *avatarRotateAnimation;
    double avatarRotationAngle;   // 当前旋转角度
    QPixmap originalAvatarPixmap; // 原始头像图像

private slots:
    void onPlayModeChanged(int index);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void openMusicFile();
    void onPlaylistItemDoubleClicked(QListWidgetItem *item);
    void uploadAvatar();                      // 上传头像槽函数
    void updateAvatarRotation();              // 更新头像旋转
    void updateProgress();                    // 更新进度条
    void onProgressSliderMoved(int position); // 处理进度条拖动
};

#endif // SETTING_H
