#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSlider>
#include <QFileDialog>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QPixmap>
#include <QToolButton>
#include <QEvent>
#include "lbutton.h"
#include "datamanager.h"
#include "messagebox.h"

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr, DataManager *dataManager = nullptr);
    ~Setting();

    Lbutton *backButton;    // 返回按钮
    //Lbutton *clearDataButton; // 清除数据按钮

    // 音乐播放器相关方法
    void playMusic();
    void pauseMusic();
    void setVolume(int volume);
    void setButtonVolume(int volume);
    void nextSong();
    void previousSong();
    void loadPlaylist();
    void updatePlaylistDisplay();
    void savePlaylistToFile();   // 保存歌单到本地
    void addSongToPlaylist(const QString &filePath);  // 添加歌曲到歌单
    void removeSongFromPlaylist();  // 从歌单中删除歌曲
    
    // 头像相关方法
    void loadAvatar();  // 加载头像
    void saveAvatarPath();  // 保存头像路径
    
    // 事件过滤器
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    DataManager *dataManager;
    MessageBox *confirmMessageBox;  // 确认对话框
    MessageBox *successMessageBox;  // 成功提示对话框

    // 音乐播放器相关控件和变量
    QMediaPlayer *musicPlayer;      // 音乐播放器
    QAudioOutput *musicAudioOutput; // 音频输出
    QSlider *volumeSlider;          // 音量滑块
    QSlider *buttonVolumeSlider;    // 按钮音效音量滑块
    QToolButton *playButton;        // 播放按钮
    QToolButton *pauseButton;       // 暂停按钮
    QToolButton *nextButton;        // 下一首按钮
    QToolButton *prevButton;        // 上一首按钮
    Lbutton *openFileButton;        // 打开文件按钮
    Lbutton *removeSongButton;      // 删除歌曲按钮
    QLabel *currentSongLabel;       // 当前歌曲标签
    QComboBox *playModeComboBox;    // 播放模式选择框
    QListWidget *playlistWidget;    // 歌单显示列表
    
    // 头像相关控件和变量
    QLabel *avatarLabel;            // 头像显示标签
    Lbutton *uploadAvatarButton;    // 上传头像按钮
    QString avatarPath;             // 头像文件路径
    const QString avatarConfigPath = "avatar.json"; // 头像配置文件路径
    
    QStringList playlist;           // 播放列表
    int currentIndex;               // 当前播放索引
    enum PlayMode {
        SingleLoop,                 // 单曲循环
        SequentialPlay,             // 顺序播放
        RandomPlay                  // 随机播放
    };
    PlayMode currentPlayMode;       // 当前播放模式
    const QString playlistFilePath = "playlist.json";  // 歌单保存路径

private slots:
    void onPlayModeChanged(int index);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void openMusicFile();
    void onPlaylistItemDoubleClicked(QListWidgetItem *item);
    void uploadAvatar();  // 上传头像槽函数
};

#endif // SETTING_H
