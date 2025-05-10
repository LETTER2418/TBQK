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
    Lbutton *clearDataButton; // 清除数据按钮

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

private:
    DataManager *dataManager;
    MessageBox *confirmMessageBox;  // 确认对话框
    MessageBox *successMessageBox;  // 成功提示对话框

    // 音乐播放器相关控件和变量
    QMediaPlayer *musicPlayer;      // 音乐播放器
    QAudioOutput *musicAudioOutput; // 音频输出
    QSlider *volumeSlider;          // 音量滑块
    QSlider *buttonVolumeSlider;    // 按钮音效音量滑块
    Lbutton *playButton;            // 播放按钮
    Lbutton *pauseButton;           // 暂停按钮
    Lbutton *nextButton;            // 下一首按钮
    Lbutton *prevButton;            // 上一首按钮
    Lbutton *openFileButton;        // 打开文件按钮
    Lbutton *removeSongButton;      // 删除歌曲按钮
    QLabel *currentSongLabel;       // 当前歌曲标签
    QComboBox *playModeComboBox;    // 播放模式选择框
    QListWidget *playlistWidget;    // 歌单显示列表
    
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
};

#endif // SETTING_H
