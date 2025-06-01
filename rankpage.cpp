#include "rankpage.h"
#include <QHeaderView>
#include <QFont>
#include <algorithm>
#include "messagebox.h"
#include <QShowEvent>

RankPage::RankPage(QWidget *parent, DataManager *dataManager_)
    : QWidget(parent), dataManager(dataManager_)
{
    setupUI();
    connectSignals();

    // 默认显示第一关的排行榜 (初始排序)
    if (levelSelector->count() > 0)
    {
        int levelId = 1;
        QVector<Ranking> rankings = dataManager->getRanking(levelId); // 获取数据
        // 初始排序 (按时间和步数)
        std::sort(rankings.begin(), rankings.end(), [&](const Ranking &a, const Ranking &b)
                  {
                if (a.penaltySeconds != b.penaltySeconds)
                    {
                        return a.penaltySeconds < b.penaltySeconds;
                    }
                return a.steps < b.steps; });
        refreshRankingList(rankings); // 传递排序后的数据
    }
}

RankPage::~RankPage()
{
}

void RankPage::setupUI()
{
    // 设置应用图标
    QIcon appIcon(":/image/taiji.png");
    this->setWindowIcon(appIcon);

    // 创建返回按钮
    backButton = new Lbutton(this, "↩️ 返回");
    backButton->move(0, 0);

    // 创建标题标签
    titleLabel = new QLabel("🏆 游戏排行榜", this);
    titleLabel->setStyleSheet("color: white;");
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // 创建关卡选择下拉框
    levelSelector = new QComboBox(this);
    for (int i = 1; i <= 12; i++)
    {
        levelSelector->addItem(QString("关卡 %1").arg(i), i);
    }

    // 设置levelSelector的字体大小
    QFont comboFont = levelSelector->font();
    comboFont.setPointSize(14);
    levelSelector->setFont(comboFont);

    // 创建清除按钮
    clearButton = new Lbutton(this, "🧹 清除本关数据");
    clearButton->setFixedWidth(140);

    // 创建排行榜表格
    rankingTable = new QTableWidget(this);
    rankingTable->setColumnCount(4);
    rankingTable->setHorizontalHeaderLabels(QStringList() << "排名" << "用户名" << "用时" << "步数");
    rankingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    rankingTable->setEditTriggers(QTableWidget::NoEditTriggers);
    rankingTable->setAlternatingRowColors(true);
    rankingTable->setStyleSheet("QTableWidget { alternate-background-color: #f0f0f0; }");
    rankingTable->horizontalHeader()->setSectionsClickable(true); // 使表头可点击
    rankingTable->setSortingEnabled(false);                       // 禁用内置排序，我们手动处理
    rankingTable->verticalHeader()->setVisible(false);
    rankingTable->setStyleSheet("font-size: 14pt;"); // 设置字体大小为 14

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *selectorLayout = new QHBoxLayout();

    // 创建标签并设置与levelSelector相同的字体大小并设置白色
    QLabel *levelLabel = new QLabel("选择关卡:", this);
    QFont labelFont = levelSelector->font();
    levelLabel->setFont(labelFont);
    levelLabel->setStyleSheet("color: white;");

    selectorLayout->addWidget(levelLabel);
    selectorLayout->addWidget(levelSelector);
    selectorLayout->addStretch();
    selectorLayout->addWidget(clearButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(selectorLayout);
    mainLayout->addWidget(rankingTable);

    // 设置主布局的边距，避免与返回按钮重叠
    mainLayout->setContentsMargins(50, 80, 50, 50);
}

void RankPage::connectSignals()
{
    // 连接下拉框选择变化的信号
    connect(levelSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index)
            {
                // 获取当前选择的关卡ID
                int levelId = levelSelector->itemData(index).toInt();

                QVector<Ranking> rankings = dataManager->getRanking(levelId); // 获取数据
                // 按当前选择的列和顺序排序
                std::sort(rankings.begin(), rankings.end(), [&](const Ranking &a, const Ranking &b)
                          {
            bool less;
            if (currentSortColumn == 2)   // 按用时排
                {
                    if (a.penaltySeconds != b.penaltySeconds)
                        {
                            less = a.penaltySeconds < b.penaltySeconds;
                        }
                    else
                        {
                            less = a.steps < b.steps; // 时间相同按步数
                        }
                }
            else if (currentSortColumn == 3)     // 按步数排
                {
                    if (a.steps != b.steps)
                        {
                            less = a.steps < b.steps;
                        }
                    else
                        {
                            less = a.penaltySeconds < b.penaltySeconds; // 步数相同按时间
                        }
                }
            else     // 其他列或默认（理论上不会到这里，因为只处理2和3）
                {
                    less = a.penaltySeconds < b.penaltySeconds;
                }
            return currentSortOrder == Qt::AscendingOrder ? less : !less; });
                refreshRankingList(rankings); // 更新显示
            });

    // 连接表头点击信号
    connect(rankingTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &RankPage::sortTable);

    // 连接清除按钮点击信号
    connect(clearButton, &Lbutton::clicked, this, &RankPage::clearCurrentLevelData);
}

// 新增：处理表头点击事件的槽函数
void RankPage::sortTable(int logicalIndex)
{
    // 只处理"用时"（索引2）和"步数"（索引3）列的点击
    if (logicalIndex == 2 || logicalIndex == 3)
    {
        if (currentSortColumn == logicalIndex)
        {
            // 如果点击的是当前排序列，则切换排序顺序
            currentSortOrder = (currentSortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        }
        else
        {
            // 如果点击的是新列，则设置新列为排序列，并重置为升序
            currentSortColumn = logicalIndex;
            currentSortOrder = Qt::AscendingOrder;
        }

        // 触发重新排序和刷新
        int levelId = levelSelector->currentData().toInt();

        QVector<Ranking> rankings = dataManager->getRanking(levelId);

        // 根据新的列和顺序排序
        std::sort(rankings.begin(), rankings.end(), [&](const Ranking &a, const Ranking &b)
                  {
                bool less;
                if (currentSortColumn == 2)   // 按用时排
                    {
                        if (a.penaltySeconds != b.penaltySeconds)
                            {
                                less = a.penaltySeconds < b.penaltySeconds;
                            }
                        else
                            {
                                less = a.steps < b.steps; // 时间相同按步数
                            }
                    }
                else     // 按步数排 (logicalIndex == 3)
                    {
                        if (a.steps != b.steps)
                            {
                                less = a.steps < b.steps;
                            }
                        else
                            {
                                less = a.penaltySeconds < b.penaltySeconds; // 步数相同按时间
                            }
                    }
                return currentSortOrder == Qt::AscendingOrder ? less : !less; });

        refreshRankingList(rankings);
    }
}

// 修改：接收排序好的数据进行显示
void RankPage::refreshRankingList(const QVector<Ranking> &rankings)
{
    // 更新表格
    rankingTable->setRowCount(rankings.size());

    for (int i = 0; i < rankings.size(); ++i)
    {
        const Ranking &entry = rankings[i];

        // 排名 (根据当前显示顺序)
        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(i + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 0, rankItem);

        // 用户名
        QTableWidgetItem *userItem = new QTableWidgetItem(entry.userId);
        userItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 1, userItem);

        // 用时
        QTableWidgetItem *timeItem = new QTableWidgetItem(formatTime(entry.penaltySeconds));
        timeItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 2, timeItem);

        // 步数
        QTableWidgetItem *stepsItem = new QTableWidgetItem(QString::number(entry.steps));
        stepsItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 3, stepsItem);
    }
}

QString RankPage::formatTime(int seconds) const
{
    int minutes = seconds / 60;
    int remainSeconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(remainSeconds, 2, 10, QChar('0'));
}

// 新增：清除当前选择关卡的排行榜数据
void RankPage::clearCurrentLevelData()
{
    int levelId = levelSelector->currentData().toInt();

    // 创建确认对话框，使用自定义MessageBox
    MessageBox *confirmBox = new MessageBox(this, true);
    confirmBox->setMessage(QString("确定要清除关卡 %1 的所有排行榜数据吗？").arg(levelId));

    // 连接确认和取消按钮
    connect(confirmBox->closeButton, &Lbutton::clicked, this, [this, confirmBox, levelId]()
            {
        // 用户确认，清除数据
        dataManager->clearRanking(levelId);

        // 刷新显示为空表格
        QVector<Ranking> emptyRankings;
        refreshRankingList(emptyRankings);

        // 使用MessageBox显示成功消息
        MessageBox *successBox = new MessageBox(this); // 使用this作为父对象
        successBox->setMessage(QString("关卡 %1 的排行榜数据已清除").arg(levelId));
        connect(successBox->closeButton, &Lbutton::clicked, successBox, &MessageBox::accept);
        successBox->exec();

        confirmBox->accept(); });

    if (confirmBox->cancelButton)
    {
        connect(confirmBox->cancelButton, &Lbutton::clicked, confirmBox, &MessageBox::reject);
    }

    // 执行确认对话框
    confirmBox->exec();
}

// 新增：实现 showEvent 以在页面显示时刷新数据
void RankPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 确保从文件加载最新数据
    dataManager->loadFromFile();

    // 刷新当前关卡的排行榜
    if (levelSelector->count() > 0)
    {
        int currentIndex = levelSelector->currentIndex();
        int levelId = levelSelector->itemData(currentIndex).toInt();

        QVector<Ranking> rankings = dataManager->getRanking(levelId);

        // 按当前排序方式排序
        std::sort(rankings.begin(), rankings.end(), [&](const Ranking &a, const Ranking &b)
                  {
                bool less;
                if (currentSortColumn == 2)   // 按用时排
                    {
                        if (a.penaltySeconds != b.penaltySeconds)
                            {
                                less = a.penaltySeconds < b.penaltySeconds;
                            }
                        else
                            {
                                less = a.steps < b.steps; // 时间相同按步数
                            }
                    }
                else if (currentSortColumn == 3)     // 按步数排
                    {
                        if (a.steps != b.steps)
                            {
                                less = a.steps < b.steps;
                            }
                        else
                            {
                                less = a.penaltySeconds < b.penaltySeconds; // 步数相同按时间
                            }
                    }
                else     // 默认按用时排序
                    {
                        if (a.penaltySeconds != b.penaltySeconds)
                            {
                                less = a.penaltySeconds < b.penaltySeconds;
                            }
                        else
                            {
                                less = a.steps < b.steps;
                            }
                    }
                return currentSortOrder == Qt::AscendingOrder ? less : !less; });

        refreshRankingList(rankings);
    }
}