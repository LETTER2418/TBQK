#include "setting.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QGroupBox>
#include <QFileInfo>
#include <QTime>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>

Setting::Setting(QWidget *parent, DataManager *dataManager_)
    : QWidget(parent), dataManager(dataManager_), currentIndex(0), currentPlayMode(SequentialPlay)
{
    // 设置应用图标
    QIcon appIcon(":/image/taiji.png");
    this->setWindowIcon(appIcon);

    // 创建返回按钮
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);

    // 创建标题标签
    QLabel *titleLabel = new QLabel("设置", this);
    QFont titleFont;
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: white; }");

    // 创建清除数据按钮
    clearDataButton = new Lbutton(this, "清除所有数据");
    
    // 创建消息框
    confirmMessageBox = new MessageBox(this, true);
    confirmMessageBox->setMessage("确定要清除所有数据吗？这将删除所有用户、地图和排行榜数据，且无法恢复！");
    
    successMessageBox = new MessageBox(this);
    successMessageBox->setMessage("所有数据已清除！");

    // 连接清除数据按钮点击事件
    connect(clearDataButton, &Lbutton::clicked, this, [this]() {
        confirmMessageBox->exec();
    });

    // 连接确认对话框的确认按钮
    connect(confirmMessageBox->closeButton, &Lbutton::clicked, this, [this]() {
        dataManager->clearAllData();
        dataManager->saveToFile();
        confirmMessageBox->accept();
        successMessageBox->exec();
    });

    connect(successMessageBox->closeButton, &Lbutton::clicked, this, [this]() {
        successMessageBox->accept();
    });

    // 连接确认对话框的取消按钮
    connect(confirmMessageBox->cancelButton, &Lbutton::clicked, this, [this]() {
        confirmMessageBox->reject(); // 关闭对话框但不执行任何清除操作
    });

    // 创建音乐播放器
    musicPlayer = new QMediaPlayer(this);
    musicAudioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(musicAudioOutput);

    // 设置初始音量
    musicAudioOutput->setVolume(0.5);

    // 创建音乐播放器控件
    QFont labelFont;
    labelFont.setPointSize(14);
    
    currentSongLabel = new QLabel("无正在播放的音乐", this);
    currentSongLabel->setFont(labelFont);
    currentSongLabel->setStyleSheet("QLabel { color: white; }");
    
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50); // 初始音量50%
    volumeSlider->setFixedWidth(200);
    volumeSlider->setMinimumHeight(30);
    
    buttonVolumeSlider = new QSlider(Qt::Horizontal, this);
    buttonVolumeSlider->setRange(0, 100);
    buttonVolumeSlider->setValue(50); // 初始音量50%
    buttonVolumeSlider->setFixedWidth(200);
    buttonVolumeSlider->setMinimumHeight(30);
    
    playButton = new Lbutton(this, "播放");
    pauseButton = new Lbutton(this, "暂停");
    nextButton = new Lbutton(this, "下一首");
    prevButton = new Lbutton(this, "上一首");
    openFileButton = new Lbutton(this, "打开音乐");
    removeSongButton = new Lbutton(this, "删除选中歌曲");
    
    playModeComboBox = new QComboBox(this);
    playModeComboBox->addItem("单曲循环");
    playModeComboBox->addItem("顺序播放");
    playModeComboBox->addItem("随机播放");
    playModeComboBox->setCurrentIndex(1); // 默认顺序播放
    playModeComboBox->setStyleSheet("QComboBox { color: white; background-color: rgba(40, 40, 40, 180); font-size: 14pt; min-height: 30px; }");
    
    // 创建歌单列表
    playlistWidget = new QListWidget(this);
    playlistWidget->setStyleSheet("QListWidget { color: white; background-color: rgba(40, 40, 40, 150); border: 1px solid rgba(255, 255, 255, 100); border-radius: 5px; font-size: 14pt; }"
                                  "QListWidget::item { padding: 10px; }"
                                  "QListWidget::item:selected { background-color: rgba(100, 100, 255, 100); }");
    playlistWidget->setMinimumHeight(200);
    playlistWidget->setAlternatingRowColors(true);

    // 创建音乐播放器布局
    QGroupBox *musicPlayerBox = new QGroupBox("音乐播放器", this);
    QFont groupBoxFont;
    groupBoxFont.setPointSize(16);
    musicPlayerBox->setFont(groupBoxFont);
    musicPlayerBox->setStyleSheet("QGroupBox { color: white; border: 1px solid white; border-radius: 5px; margin-top: 15px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 10px 0 10px; }");
    
    QVBoxLayout *musicLayout = new QVBoxLayout();
    musicLayout->setSpacing(15);
    
    QHBoxLayout *controlButtonLayout = new QHBoxLayout();
    controlButtonLayout->addWidget(playButton);
    controlButtonLayout->addWidget(pauseButton);
    controlButtonLayout->addWidget(prevButton);
    controlButtonLayout->addWidget(nextButton);
    controlButtonLayout->setSpacing(15);
    
    QHBoxLayout *fileControlLayout = new QHBoxLayout();
    fileControlLayout->addWidget(openFileButton);
    fileControlLayout->addWidget(removeSongButton);
    fileControlLayout->addWidget(playModeComboBox);
    fileControlLayout->setSpacing(15);
    
    QHBoxLayout *volumeLayout = new QHBoxLayout();
    QLabel *volumeLabel = new QLabel("音乐音量:", this);
    volumeLabel->setFont(labelFont);
    volumeLabel->setStyleSheet("QLabel { color: white; }");
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(volumeSlider);
    
    QHBoxLayout *buttonVolumeLayout = new QHBoxLayout();
    QLabel *buttonVolumeLabel = new QLabel("按钮音效音量:", this);
    buttonVolumeLabel->setFont(labelFont);
    buttonVolumeLabel->setStyleSheet("QLabel { color: white; }");
    buttonVolumeLayout->addWidget(buttonVolumeLabel);
    buttonVolumeLayout->addWidget(buttonVolumeSlider);
    
    // 添加歌单标题
    QLabel *playlistLabel = new QLabel("当前歌单:", this);
    playlistLabel->setFont(labelFont);
    playlistLabel->setStyleSheet("QLabel { color: white; font-weight: bold; }");
    
    musicLayout->addWidget(currentSongLabel);
    musicLayout->addLayout(controlButtonLayout);
    musicLayout->addLayout(fileControlLayout);
    musicLayout->addLayout(volumeLayout);
    musicLayout->addLayout(buttonVolumeLayout);
    musicLayout->addWidget(playlistLabel);
    musicLayout->addWidget(playlistWidget);
    
    musicPlayerBox->setLayout(musicLayout);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(musicPlayerBox);
    mainLayout->addWidget(clearDataButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->setSpacing(20);
    
    // 设置边距
    mainLayout->setContentsMargins(50, 80, 50, 50);

    // 连接音乐播放器信号和槽
    connect(playButton, &Lbutton::clicked, this, &Setting::playMusic);
    connect(pauseButton, &Lbutton::clicked, this, &Setting::pauseMusic);
    connect(nextButton, &Lbutton::clicked, this, &Setting::nextSong);
    connect(prevButton, &Lbutton::clicked, this, &Setting::previousSong);
    connect(openFileButton, &Lbutton::clicked, this, &Setting::openMusicFile);
    connect(removeSongButton, &Lbutton::clicked, this, &Setting::removeSongFromPlaylist);
    connect(volumeSlider, &QSlider::valueChanged, this, &Setting::setVolume);
    connect(buttonVolumeSlider, &QSlider::valueChanged, this, &Setting::setButtonVolume);
    connect(playModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Setting::onPlayModeChanged);
    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, &Setting::onMediaStatusChanged);
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &Setting::onPlaylistItemDoubleClicked);

    // 加载保存的歌单
    loadPlaylist();
}

Setting::~Setting()
{
    delete confirmMessageBox;
    delete successMessageBox;
    delete musicPlayer;
    delete musicAudioOutput;
}

void Setting::playMusic()
{
    if (playlist.isEmpty()) {
        openMusicFile();
    } else if (musicPlayer->playbackState() != QMediaPlayer::PlayingState) {
        musicPlayer->play();
    }
}

void Setting::pauseMusic()
{
    if (musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
        musicPlayer->pause();
    }
}

void Setting::setVolume(int volume)
{
    musicAudioOutput->setVolume(volume / 100.0);
}

void Setting::setButtonVolume(int volume)
{
    // 遍历所有的Lbutton并设置它们的音频输出音量
    QList<Lbutton*> buttons = this->findChildren<Lbutton*>();
    for (Lbutton* button : buttons) {
        button->setButtonVolume(volume / 100.0);
    }
}

void Setting::nextSong()
{
    if (playlist.isEmpty()) return;
    
    if (currentPlayMode == RandomPlay) {
        currentIndex = QRandomGenerator::global()->bounded(playlist.size());
    } else {
        currentIndex = (currentIndex + 1) % playlist.size();
    }
    
    musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
    QFileInfo fileInfo(playlist.at(currentIndex));
    currentSongLabel->setText("正在播放: " + fileInfo.fileName());
    musicPlayer->play();
    
    // 更新歌单中的选中项
    playlistWidget->setCurrentRow(currentIndex);
}

void Setting::previousSong()
{
    if (playlist.isEmpty()) return;
    
    if (currentPlayMode == RandomPlay) {
        currentIndex = QRandomGenerator::global()->bounded(playlist.size());
    } else {
        currentIndex = (currentIndex - 1 + playlist.size()) % playlist.size();
    }
    
    musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
    QFileInfo fileInfo(playlist.at(currentIndex));
    currentSongLabel->setText("正在播放: " + fileInfo.fileName());
    musicPlayer->play();
    
    // 更新歌单中的选中项
    playlistWidget->setCurrentRow(currentIndex);
}

void Setting::loadPlaylist()
{
    QFile file(playlistFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        
        if (doc.isArray()) {
            QJsonArray array = doc.array();
            playlist.clear();
            
            for (int i = 0; i < array.size(); i++) {
                QString filePath = array[i].toString();
                // 检查文件是否存在
                QFileInfo fileInfo(filePath);
                if (fileInfo.exists()) {
                    playlist.append(filePath);
                }
            }
            
            updatePlaylistDisplay();
        }
    }
}

void Setting::updatePlaylistDisplay()
{
    playlistWidget->clear();
    
    for (const QString &filePath : playlist) {
        QFileInfo fileInfo(filePath);
        playlistWidget->addItem(fileInfo.fileName());
    }
    
    if (currentIndex >= 0 && currentIndex < playlist.size()) {
        playlistWidget->setCurrentRow(currentIndex);
    }
}

void Setting::openMusicFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "选择音乐文件",
        QDir::homePath(),
        "音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a)"
    );
    
    if (!files.isEmpty()) {
        for (const QString &file : files) {
            // 添加到歌单并保存
            addSongToPlaylist(file);
        }
        
        // 如果之前没有播放音乐，则开始播放第一首添加的歌曲
        if (musicPlayer->playbackState() != QMediaPlayer::PlayingState) {
            currentIndex = playlist.indexOf(files.first());
            if (currentIndex == -1) currentIndex = 0;
            
            musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
            QFileInfo fileInfo(playlist.at(currentIndex));
            currentSongLabel->setText("正在播放: " + fileInfo.fileName());
            musicPlayer->play();
            
            // 更新歌单显示
            playlistWidget->setCurrentRow(currentIndex);
        }
    }
}

void Setting::onPlayModeChanged(int index)
{
    switch (index) {
    case 0:
        currentPlayMode = SingleLoop;
        break;
    case 1:
        currentPlayMode = SequentialPlay;
        break;
    case 2:
        currentPlayMode = RandomPlay;
        break;
    }
}

void Setting::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        switch (currentPlayMode) {
        case SingleLoop:
            // 重新播放当前歌曲
            musicPlayer->setPosition(0);
            musicPlayer->play();
            break;
        case SequentialPlay:
            // 播放下一首
            nextSong();
            break;
        case RandomPlay:
            // 随机播放
            currentIndex = QRandomGenerator::global()->bounded(playlist.size());
            if (!playlist.isEmpty()) {
                musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
                QFileInfo fileInfo(playlist.at(currentIndex));
                currentSongLabel->setText("正在播放: " + fileInfo.fileName());
                musicPlayer->play();
                
                // 更新歌单中的选中项
                playlistWidget->setCurrentRow(currentIndex);
            }
            break;
        }
    }
}

void Setting::onPlaylistItemDoubleClicked(QListWidgetItem *item)
{
    int row = playlistWidget->row(item);
    if (row >= 0 && row < playlist.size()) {
        currentIndex = row;
        musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
        QFileInfo fileInfo(playlist.at(currentIndex));
        currentSongLabel->setText("正在播放: " + fileInfo.fileName());
        musicPlayer->play();
    }
}

void Setting::savePlaylistToFile()
{
    QJsonArray array;
    for (const QString &filePath : playlist) {
        array.append(filePath);
    }
    
    QJsonDocument doc(array);
    QFile file(playlistFilePath);
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void Setting::addSongToPlaylist(const QString &filePath)
{
    // 检查文件是否已经在歌单中
    if (!playlist.contains(filePath)) {
        playlist.append(filePath);
        QFileInfo fileInfo(filePath);
        playlistWidget->addItem(fileInfo.fileName());
        
        // 保存更新后的歌单
        savePlaylistToFile();
    }
}

void Setting::removeSongFromPlaylist()
{
    int row = playlistWidget->currentRow();
    if (row >= 0 && row < playlist.size()) {
        // 如果正在播放当前要删除的歌曲，先切换到下一首
        if (row == currentIndex && musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
            nextSong();
        }
        
        // 如果当前播放歌曲在要删除歌曲之后，需要调整currentIndex
        if (currentIndex > row) {
            currentIndex--;
        }
        
        // 从列表中移除
        playlist.removeAt(row);
        delete playlistWidget->takeItem(row);
        
        // 保存更新后的歌单
        savePlaylistToFile();
        
        // 如果删除后歌单为空，更新UI
        if (playlist.isEmpty()) {
            musicPlayer->stop();
            currentSongLabel->setText("无正在播放的音乐");
            currentIndex = 0;
        }
    }
}
