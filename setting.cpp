#include "setting.h"

Setting::Setting(QWidget *parent, DataManager *dataManager_)
    : QWidget(parent), dataManager(dataManager_), currentIndex(0), currentPlayMode(SequentialPlay),
      avatarRotateAnimation(nullptr), avatarRotationAngle(0.0)
{
    // 设置应用图标
    QIcon appIcon(":/image/taiji.png");
    this->setWindowIcon(appIcon);

    // 创建返回按钮
    backButton = new Lbutton(this, "↩️ 返回");
    backButton->move(0, 0);

    // 创建清除数据按钮
    // clearDataButton = new Lbutton(this, "清除所有数据");

    // 创建消息框
    confirmMessageBox = new MessageBox(this, true);
    confirmMessageBox->setMessage("确定要清除所有数据吗？这将删除所有用户、地图和排行榜数据，且无法恢复！");

    successMessageBox = new MessageBox(this);
    successMessageBox->setMessage("所有数据已清除！");

    // 连接清除数据按钮点击事件
    // connect(clearDataButton, &Lbutton::clicked, this, [this]() {
    //     confirmMessageBox->exec();
    // });

    // 连接确认对话框的确认按钮
    connect(confirmMessageBox->closeButton, &Lbutton::clicked, this, [this]()
            {
        dataManager->clearAllData();
        dataManager->saveToFile();
        confirmMessageBox->accept();
        successMessageBox->exec(); });

    connect(successMessageBox->closeButton, &Lbutton::clicked, this, [this]()
            { successMessageBox->accept(); });

    // 连接确认对话框的取消按钮
    connect(confirmMessageBox->cancelButton, &Lbutton::clicked, this, [this]()
            {
                confirmMessageBox->reject(); // 关闭对话框但不执行任何清除操作
            });

    // 创建音乐播放器
    musicPlayer = new QMediaPlayer(this);
    musicAudioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(musicAudioOutput);

    // 设置初始音量
    musicAudioOutput->setVolume(0);

    // 创建音乐播放器控件
    QFont labelFont;
    labelFont.setPointSize(14);

    currentSongLabel = new QLabel("无正在播放的音乐", this);
    currentSongLabel->setFont(labelFont);
    currentSongLabel->setStyleSheet("QLabel { color: white; }");

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(0); // 初始音量0%
    setVolume(0);
    volumeSlider->setFixedWidth(200);
    volumeSlider->setMinimumHeight(30);

    buttonVolumeSlider = new QSlider(Qt::Horizontal, this);
    buttonVolumeSlider->setRange(0, 100);
    buttonVolumeSlider->setValue(0); // 初始音量0%
    setButtonVolume(0);
    buttonVolumeSlider->setFixedWidth(200);
    buttonVolumeSlider->setMinimumHeight(30);

    // 创建音乐控制图标按钮
    playButton = new QToolButton(this);
    playButton->setIcon(QIcon(":/image/play.png"));
    playButton->setIconSize(QSize(40, 40));
    playButton->setToolTip("播放");
    playButton->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    playButton->setFixedSize(50, 50);
    playButton->installEventFilter(this);

    pauseButton = new QToolButton(this);
    pauseButton->setIcon(QIcon(":/image/pause.png"));
    pauseButton->setIconSize(QSize(40, 40));
    pauseButton->setToolTip("暂停");
    pauseButton->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    pauseButton->setFixedSize(50, 50);
    pauseButton->hide(); // 初始隐藏暂停按钮
    pauseButton->installEventFilter(this);

    prevButton = new QToolButton(this);
    prevButton->setIcon(QIcon(":/image/last.png"));
    prevButton->setIconSize(QSize(40, 40));
    prevButton->setToolTip("上一首");
    prevButton->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    prevButton->setFixedSize(50, 50);
    prevButton->installEventFilter(this);

    nextButton = new QToolButton(this);
    nextButton->setIcon(QIcon(":/image/next.png"));
    nextButton->setIconSize(QSize(40, 40));
    nextButton->setToolTip("下一首");
    nextButton->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    nextButton->setFixedSize(50, 50);
    nextButton->installEventFilter(this);

    openFileButton = new Lbutton(this, "🎵 打开音乐");
    openFileButton->enableClickEffect(true);
    removeSongButton = new Lbutton(this, "🗑️ 删除歌曲");
    removeSongButton->enableClickEffect(true);

    playModeComboBox = new QComboBox(this);
    playModeComboBox->addItem("单曲循环");
    playModeComboBox->addItem("顺序播放");
    playModeComboBox->addItem("随机播放");
    playModeComboBox->setCurrentIndex(1); // 默认顺序播放
    playModeComboBox->setStyleSheet("QComboBox { color: white; background-color: rgb(100, 100, 255); font-size: 14pt; min-height: 30px; max-width: 105px; padding-left: 10px; }"
                                    "QComboBox QAbstractItemView { background-color: rgb(40, 40, 40); color: white; }");

    // 创建歌单列表
    playlistWidget = new QListWidget(this);
    playlistWidget->setStyleSheet("QListWidget { color: white; background-color: rgba(40, 40, 40, 150); border: 1px solid rgba(255, 255, 255, 100); border-radius: 5px; font-size: 14pt; }"
                                  "QListWidget::item { padding: 10px; }"
                                  "QListWidget::item:selected { background-color: rgba(100, 100, 255, 100); }");
    playlistWidget->setMinimumHeight(200);
    playlistWidget->setAlternatingRowColors(true);

    // 创建头像相关控件
    avatarLabel = new QLabel(this);
    avatarLabel->setFixedSize(120, 120);
    avatarLabel->setStyleSheet("QLabel { background-color: rgba(40, 40, 40, 150); border: 2px solid white; border-radius: 60px; }");
    avatarLabel->setAlignment(Qt::AlignCenter);

    // 为头像标签安装事件过滤器
    avatarLabel->installEventFilter(this);

    // 创建旋转动画
    avatarRotateAnimation = new QPropertyAnimation(this, "avatarRotationAngle");
    avatarRotateAnimation->setDuration(500);                       // 动画持续时间，单位毫秒
    avatarRotateAnimation->setEasingCurve(QEasingCurve::OutCubic); // 动画缓动曲线
    connect(avatarRotateAnimation, &QPropertyAnimation::valueChanged, this, [this](const QVariant &value)
            {
        // 更新旋转角度并重新绘制头像
        avatarRotationAngle = value.toDouble();
        updateAvatarRotation(); });

    uploadAvatarButton = new Lbutton(this, "🖼️ 更换头像");
    uploadAvatarButton->enableClickEffect(true);

    // 创建头像布局
    QVBoxLayout *avatarLayout = new QVBoxLayout();
    avatarLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    avatarLayout->addWidget(uploadAvatarButton, 0, Qt::AlignCenter);
    avatarLayout->setSpacing(10);

    QFont groupBoxFont;
    groupBoxFont.setPointSize(16);

    QGroupBox *avatarBox = new QGroupBox("👤 用户头像", this);
    avatarBox->setFont(groupBoxFont);
    avatarBox->setStyleSheet("QGroupBox { color: white; border: 1px solid white; border-radius: 5px; margin-top: 15px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 10px 0 10px; }");
    avatarBox->setLayout(avatarLayout);
    avatarBox->setFixedWidth(200);

    // 创建音乐播放器布局
    QGroupBox *musicPlayerBox = new QGroupBox("🎧 音乐播放器", this);
    musicPlayerBox->setFont(groupBoxFont);
    musicPlayerBox->setStyleSheet("QGroupBox { color: white; border: 1px solid white; border-radius: 5px; margin-top: 15px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 10px 0 10px; }");

    QVBoxLayout *musicLayout = new QVBoxLayout();
    musicLayout->setContentsMargins(20, 20, 20, 20);
    musicLayout->setSpacing(15);

    // 创建布局
    QHBoxLayout *controlButtonLayout = new QHBoxLayout();
    controlButtonLayout->setAlignment(Qt::AlignCenter); // 设置整体布局居中

    // 创建播放/暂停按钮容器
    QWidget *playPauseContainer = new QWidget(this);
    playPauseContainer->setFixedSize(100, 100);
    QVBoxLayout *playPauseLayout = new QVBoxLayout(playPauseContainer);
    playPauseLayout->setContentsMargins(0, 0, 0, 0);
    playPauseLayout->addWidget(playButton);
    playPauseLayout->addWidget(pauseButton);
    playPauseLayout->setAlignment(Qt::AlignCenter); // 设置按钮在容器中居中

    controlButtonLayout->addStretch(1);
    controlButtonLayout->addWidget(prevButton);
    controlButtonLayout->addStretch(1);
    controlButtonLayout->addWidget(playPauseContainer);
    controlButtonLayout->addStretch(1);
    controlButtonLayout->addWidget(nextButton);
    controlButtonLayout->addStretch(1);

    QHBoxLayout *fileControlLayout = new QHBoxLayout();
    fileControlLayout->addWidget(openFileButton);
    fileControlLayout->addWidget(removeSongButton);
    fileControlLayout->addWidget(playModeComboBox);
    // fileControlLayout->setSpacing(40);

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

    musicLayout->addWidget(playlistLabel);
    musicLayout->addWidget(playlistWidget);
    musicLayout->addWidget(currentSongLabel);
    musicLayout->addLayout(controlButtonLayout);
    musicLayout->addLayout(fileControlLayout);
    musicLayout->addLayout(volumeLayout);
    musicLayout->addLayout(buttonVolumeLayout);

    musicPlayerBox->setLayout(musicLayout);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建水平布局来放置头像和音乐播放器
    QHBoxLayout *topContentLayout = new QHBoxLayout();
    topContentLayout->addWidget(avatarBox);
    topContentLayout->addWidget(musicPlayerBox);

    mainLayout->addLayout(topContentLayout);
    // mainLayout->addWidget(clearDataButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->setSpacing(20);

    // 设置边距
    mainLayout->setContentsMargins(50, 80, 50, 50);

    // 连接音乐播放器信号和槽
    connect(playButton, &QToolButton::clicked, this, [this]()
            {
        playMusic();
        playButton->hide();
        pauseButton->show(); });

    connect(pauseButton, &QToolButton::clicked, this, [this]()
            {
        pauseMusic();
        pauseButton->hide();
        playButton->show(); });

    connect(nextButton, &QToolButton::clicked, this, &Setting::nextSong);
    connect(prevButton, &QToolButton::clicked, this, &Setting::previousSong);
    connect(openFileButton, &Lbutton::clicked, this, &Setting::openMusicFile);
    connect(removeSongButton, &Lbutton::clicked, this, &Setting::removeSongFromPlaylist);
    connect(volumeSlider, &QSlider::valueChanged, this, &Setting::setVolume);
    connect(buttonVolumeSlider, &QSlider::valueChanged, this, &Setting::setButtonVolume);
    connect(playModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Setting::onPlayModeChanged);
    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, &Setting::onMediaStatusChanged);
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &Setting::onPlaylistItemDoubleClicked);

    // 连接头像上传按钮信号
    connect(uploadAvatarButton, &Lbutton::clicked, this, &Setting::uploadAvatar);

    // 加载头像
    loadAvatar();

    // 加载保存的歌单
    loadPlaylist();

    // 监听播放状态变化
    connect(musicPlayer, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state)
            {
        if (state == QMediaPlayer::PlayingState)
            {
                playButton->hide();
                pauseButton->show();
            }
        else
            {
                pauseButton->hide();
                playButton->show();
            } });
}

Setting::~Setting()
{
    delete confirmMessageBox;
    delete successMessageBox;
    delete musicPlayer;
    delete musicAudioOutput;

    // 清理动画
    delete avatarRotateAnimation;
}

void Setting::playMusic()
{
    if (playlist.isEmpty())
    {
        openMusicFile();
    }
    else if (musicPlayer->playbackState() != QMediaPlayer::PlayingState)
    {
        // 如果当前没有正在播放的歌曲（source为空），则选择第一首歌曲
        if (musicPlayer->source().isEmpty() && !playlist.isEmpty())
        {
            currentIndex = 0;
            musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
            QFileInfo fileInfo(playlist.at(currentIndex));
            currentSongLabel->setText("正在播放: " + fileInfo.baseName());
            playlistWidget->setCurrentRow(currentIndex);
        }
        musicPlayer->play();
    }
}

void Setting::pauseMusic()
{
    if (musicPlayer->playbackState() == QMediaPlayer::PlayingState)
    {
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
    QList<Lbutton *> buttons = this->findChildren<Lbutton *>();
    for (Lbutton *button : buttons)
    {
        button->setButtonVolume(volume / 100.0);
    }
}

void Setting::nextSong()
{
    if (playlist.isEmpty())
    {
        return;
    }

    if (currentPlayMode == RandomPlay)
    {
        currentIndex = QRandomGenerator::global()->bounded(playlist.size());
    }
    else
    {
        currentIndex = (currentIndex + 1) % playlist.size();
    }

    musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
    QFileInfo fileInfo(playlist.at(currentIndex));
    currentSongLabel->setText("正在播放: " + fileInfo.baseName());
    musicPlayer->play();

    // 更新歌单中的选中项
    playlistWidget->setCurrentRow(currentIndex);
}

void Setting::previousSong()
{
    if (playlist.isEmpty())
    {
        return;
    }

    if (currentPlayMode == RandomPlay)
    {
        currentIndex = QRandomGenerator::global()->bounded(playlist.size());
    }
    else
    {
        currentIndex = (currentIndex - 1 + playlist.size()) % playlist.size();
    }

    musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
    QFileInfo fileInfo(playlist.at(currentIndex));
    currentSongLabel->setText("正在播放: " + fileInfo.baseName());
    musicPlayer->play();

    // 更新歌单中的选中项
    playlistWidget->setCurrentRow(currentIndex);
}

void Setting::loadPlaylist()
{
    QFile file(playlistFilePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (doc.isArray())
        {
            QJsonArray array = doc.array();
            playlist.clear();

            for (int i = 0; i < array.size(); i++)
            {
                QString filePath = array[i].toString();
                // 检查文件是否存在
                QFileInfo fileInfo(filePath);
                if (fileInfo.exists())
                {
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

    for (const QString &filePath : playlist)
    {
        QFileInfo fileInfo(filePath);
        playlistWidget->addItem(fileInfo.baseName());
    }

    if (currentIndex >= 0 && currentIndex < playlist.size())
    {
        playlistWidget->setCurrentRow(currentIndex);
    }
}

void Setting::openMusicFile()
{
    // 根据用户名选择默认目录
    QString defaultDir;
    QString username = qgetenv("USERNAME");

    if (username == "LETTER")
    {
        defaultDir = QString("D:/Programming/QtProject/TBQK/music");
    }
    else
    {
        // 如果不是LETTER用户，使用C盘用户的音乐文件夹
        defaultDir = QDir::homePath();
    }

    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "选择音乐文件",
        defaultDir,
        "音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a)");

    if (!files.isEmpty())
    {
        for (const QString &file : files)
        {
            // 添加到歌单并保存
            addSongToPlaylist(file);
        }

        // 如果之前没有播放音乐，则开始播放第一首添加的歌曲
        if (musicPlayer->playbackState() != QMediaPlayer::PlayingState)
        {
            currentIndex = playlist.indexOf(files.first());
            if (currentIndex == -1)
            {
                currentIndex = 0;
            }

            musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
            QFileInfo fileInfo(playlist.at(currentIndex));
            currentSongLabel->setText("正在播放: " + fileInfo.baseName());
            musicPlayer->play();

            // 更新歌单显示
            playlistWidget->setCurrentRow(currentIndex);
        }
    }
}

void Setting::onPlayModeChanged(int index)
{
    switch (index)
    {
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
    if (status == QMediaPlayer::EndOfMedia)
    {
        switch (currentPlayMode)
        {
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
            if (!playlist.isEmpty())
            {
                musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
                QFileInfo fileInfo(playlist.at(currentIndex));
                currentSongLabel->setText("正在播放: " + fileInfo.baseName());
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
    if (row >= 0 && row < playlist.size())
    {
        currentIndex = row;
        musicPlayer->setSource(QUrl::fromLocalFile(playlist.at(currentIndex)));
        QFileInfo fileInfo(playlist.at(currentIndex));
        currentSongLabel->setText("正在播放: " + fileInfo.baseName());
        musicPlayer->play();
    }
}

void Setting::savePlaylistToFile()
{
    QJsonArray array;
    for (const QString &filePath : playlist)
    {
        array.append(filePath);
    }

    QJsonDocument doc(array);
    QFile file(playlistFilePath);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
}

void Setting::addSongToPlaylist(const QString &filePath)
{
    // 检查文件是否已经在歌单中
    if (!playlist.contains(filePath))
    {
        playlist.append(filePath);
        QFileInfo fileInfo(filePath);
        playlistWidget->addItem(fileInfo.baseName());

        // 保存更新后的歌单
        savePlaylistToFile();
    }
}

void Setting::removeSongFromPlaylist()
{
    int row = playlistWidget->currentRow();
    if (row >= 0 && row < playlist.size())
    {
        // 如果正在播放当前要删除的歌曲，先切换到下一首
        if (row == currentIndex && musicPlayer->playbackState() == QMediaPlayer::PlayingState)
        {
            nextSong();
        }

        // 如果当前播放歌曲在要删除歌曲之后，需要调整currentIndex
        if (currentIndex > row)
        {
            currentIndex--;
        }

        // 从列表中移除
        playlist.removeAt(row);
        delete playlistWidget->takeItem(row);

        // 保存更新后的歌单
        savePlaylistToFile();

        // 如果删除后歌单为空，更新UI
        if (playlist.isEmpty())
        {
            musicPlayer->stop();
            currentSongLabel->setText("无正在播放的音乐");
            currentIndex = 0;
        }
    }
}

void Setting::uploadAvatar()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择头像图片",
        QString("D:\\Image\\Pictures\\好看"),
        "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (!filePath.isEmpty())
    {
        // 加载并显示头像
        QPixmap pixmap(filePath);
        if (!pixmap.isNull())
        {
            // 保存临时头像路径（仍然保存原始路径用于显示）
            avatarPath = filePath;

            // 确保头像尺寸与容器尺寸一致
            int size = qMin(avatarLabel->width(), avatarLabel->height());

            // 创建一个正方形的目标矩形
            QRect targetRect(0, 0, size, size);

            // 计算源图像的缩放比例，确保完全覆盖目标区域
            double widthRatio = (double)size / pixmap.width();
            double heightRatio = (double)size / pixmap.height();
            double scaleFactor = qMax(widthRatio, heightRatio);

            // 缩放图像，确保覆盖整个目标区域
            QPixmap scaledPixmap = pixmap.scaled(
                pixmap.width() * scaleFactor,
                pixmap.height() * scaleFactor,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

            // 计算居中裁剪的起始位置
            int x = (scaledPixmap.width() - size) / 2;
            int y = (scaledPixmap.height() - size) / 2;

            // 裁剪为正方形
            QPixmap squarePixmap = scaledPixmap.copy(x, y, size, size);

            // 创建圆形遮罩
            QPixmap roundedPixmap(size, size);
            roundedPixmap.fill(Qt::transparent);

            QPainter painter(&roundedPixmap);
            painter.setRenderHint(QPainter::Antialiasing);

            // 创建圆形裁剪路径
            QPainterPath path;
            path.addEllipse(0, 0, size, size);
            painter.setClipPath(path);

            // 绘制头像
            painter.drawPixmap(targetRect, squarePixmap);
            painter.end();

            // 保存原始圆形头像以便旋转
            originalAvatarPixmap = roundedPixmap;

            // 显示头像
            avatarLabel->setPixmap(roundedPixmap);

            // 将未处理的原始头像存储到DataManager中以便保存文件
            if (dataManager)
            {
                // 临时保存原始头像
                dataManager->setAvatarPath(filePath);

                // 仅在用户已登录时保存头像文件
                if (!dataManager->getCurrentUserId().isEmpty())
                {
                    // 保存头像文件
                    dataManager->saveAvatarFile(dataManager->getCurrentUserId(), pixmap);
                }
            }
        }
    }
}

void Setting::loadAvatar()
{
    // 从DataManager中读取头像
    if (dataManager && !dataManager->getCurrentUserId().isEmpty())
    {
        // 直接加载头像文件
        QPixmap pixmap = dataManager->loadAvatarFile(dataManager->getCurrentUserId());

        if (!pixmap.isNull())
        {
            // 确保头像尺寸与容器尺寸一致
            int size = qMin(avatarLabel->width(), avatarLabel->height());

            // 创建一个正方形的目标矩形
            QRect targetRect(0, 0, size, size);

            // 计算源图像的缩放比例，确保完全覆盖目标区域
            double widthRatio = (double)size / pixmap.width();
            double heightRatio = (double)size / pixmap.height();
            double scaleFactor = qMax(widthRatio, heightRatio);

            // 缩放图像，确保覆盖整个目标区域
            QPixmap scaledPixmap = pixmap.scaled(
                pixmap.width() * scaleFactor,
                pixmap.height() * scaleFactor,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

            // 计算居中裁剪的起始位置
            int x = (scaledPixmap.width() - size) / 2;
            int y = (scaledPixmap.height() - size) / 2;

            // 裁剪为正方形
            QPixmap squarePixmap = scaledPixmap.copy(x, y, size, size);

            // 创建圆形遮罩
            QPixmap roundedPixmap(size, size);
            roundedPixmap.fill(Qt::transparent);

            // 绑定到一个 QPixmap 对象，创建一个离屏绘图环境
            QPainter painter(&roundedPixmap);
            painter.setRenderHint(QPainter::Antialiasing);

            // 创建圆形裁剪路径
            QPainterPath path;
            path.addEllipse(0, 0, size, size);
            painter.setClipPath(path);

            // 绘制头像
            painter.drawPixmap(targetRect, squarePixmap);
            painter.end();

            // 保存原始圆形头像以便旋转
            originalAvatarPixmap = roundedPixmap;

            // 显示头像
            avatarLabel->setPixmap(roundedPixmap);
            return;
        }
    }

    // 默认显示空头像或默认头像
    avatarLabel->setText("无头像");
    avatarLabel->setStyleSheet("QLabel { color: white; background-color: rgba(40, 40, 40, 150); border: 2px solid white; border-radius: 60px; }");
    originalAvatarPixmap = QPixmap(); // 清空原始头像
}

// 添加事件过滤器实现
bool Setting::eventFilter(QObject *watched, QEvent *event)
{
    // 检查是否是头像标签
    if (watched == avatarLabel)
    {
        if (event->type() == QEvent::Enter)
        {
            // 鼠标进入时旋转头像
            avatarRotateAnimation->stop();
            avatarRotateAnimation->setStartValue(avatarRotationAngle);
            avatarRotateAnimation->setEndValue(180.0);
            avatarRotateAnimation->start();
            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开时恢复头像
            avatarRotateAnimation->stop();
            avatarRotateAnimation->setStartValue(avatarRotationAngle);
            avatarRotateAnimation->setEndValue(0.0);
            avatarRotateAnimation->start();
            return true;
        }
    }

    // 检查是否是我们关注的按钮
    QToolButton *button = qobject_cast<QToolButton *>(watched);
    if (button && (button == playButton || button == pauseButton ||
                   button == prevButton || button == nextButton))
    {

        if (event->type() == QEvent::Enter)
        {
            // 鼠标进入时放大图标
            button->setIconSize(QSize(48, 48));
            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开时恢复图标大小
            button->setIconSize(QSize(40, 40));
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

// 设置旋转角度并触发更新
void Setting::setAvatarRotationAngle(double angle)
{
    avatarRotationAngle = angle;
    updateAvatarRotation();
}

// 更新头像旋转显示
void Setting::updateAvatarRotation()
{
    // 如果没有头像或原始头像未保存，则不执行旋转
    if (avatarLabel->pixmap(Qt::ReturnByValue).isNull() || originalAvatarPixmap.isNull())
    {
        return;
    }

    // 获取头像尺寸
    int size = qMin(avatarLabel->width(), avatarLabel->height());

    // 创建临时画布用于旋转
    QPixmap tempPixmap(size, size);
    tempPixmap.fill(Qt::transparent);

    // 创建变换矩阵进行旋转
    QTransform transform;
    transform.translate(size / 2, size / 2);   // 移动到中心点
    transform.rotate(avatarRotationAngle);     // 旋转
    transform.translate(-size / 2, -size / 2); // 移回原位置

    // 在临时画布上绘制旋转后的图像
    QPainter tempPainter(&tempPixmap);
    tempPainter.setRenderHint(QPainter::Antialiasing);
    tempPainter.setRenderHint(QPainter::SmoothPixmapTransform);
    tempPainter.setTransform(transform);

    // 计算原始图像在临时画布上的绘制位置，保证居中
    QRect sourceRect = originalAvatarPixmap.rect();
    QRect targetRect(0, 0, size, size);

    // 在临时画布上绘制原始图像（不应用遮罩）
    tempPainter.drawPixmap(targetRect, originalAvatarPixmap, sourceRect);
    tempPainter.end();

    // 创建最终的圆形头像
    QPixmap finalPixmap(size, size);
    finalPixmap.fill(Qt::transparent);

    QPainter finalPainter(&finalPixmap);
    finalPainter.setRenderHint(QPainter::Antialiasing);

    // 创建圆形路径
    QPainterPath path;
    path.addEllipse(0, 0, size, size);

    // 设置裁剪路径，只绘制圆形区域内的内容
    finalPainter.setClipPath(path);

    // 绘制旋转后的图像到最终画布，保证圆形
    finalPainter.drawPixmap(0, 0, tempPixmap);
    finalPainter.end();

    // 设置最终的圆形头像
    avatarLabel->setPixmap(finalPixmap);
}

void Setting::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadAvatar();
}
