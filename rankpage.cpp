#include "rankpage.h"
#include <QHeaderView>
#include <QFont>

RankPage::RankPage(QWidget *parent, DataManager *dataManager_)
    : QWidget(parent), dataManager(dataManager_)
{
    setupUI();
    connectSignals();
    
    // 默认显示第一关的排行榜
    if (levelSelector->count() > 0) {
        refreshRankingList(1);
    }
}

RankPage::~RankPage()
{
}

void RankPage::setupUI()
{
    // 创建返回按钮
    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);
    
    // 创建标题标签
    titleLabel = new QLabel("游戏排行榜", this);
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // 创建关卡选择下拉框
    levelSelector = new QComboBox(this);
    for (int i = 1; i <= 12; i++) {
        levelSelector->addItem(QString("关卡 %1").arg(i), i);
    }
    
    // 创建排行榜表格
    rankingTable = new QTableWidget(this);
    rankingTable->setColumnCount(3);
    rankingTable->setHorizontalHeaderLabels(QStringList() << "排名" << "用户名" << "用时");
    rankingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    rankingTable->setEditTriggers(QTableWidget::NoEditTriggers);
    rankingTable->setAlternatingRowColors(true);
    rankingTable->setStyleSheet("QTableWidget { alternate-background-color: #f0f0f0; }");
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *selectorLayout = new QHBoxLayout();
    selectorLayout->addWidget(new QLabel("选择关卡:"));
    selectorLayout->addWidget(levelSelector);
    selectorLayout->addStretch();
    
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(selectorLayout);
    mainLayout->addWidget(rankingTable);
    
    // 设置主布局的边距，避免与返回按钮重叠
    mainLayout->setContentsMargins(50, 80, 50, 50);
}

void RankPage::connectSignals()
{
    connect(levelSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [this](int index) {
                int levelId = levelSelector->itemData(index).toInt();
                refreshRankingList(levelId);
            });
}

void RankPage::refreshRankingList(int levelId)
{
    // 获取指定关卡的排行榜数据
    QVector<Ranking> rankings = dataManager->getRanking(levelId);
    
    // 更新表格
    rankingTable->setRowCount(rankings.size());
    
    for (int i = 0; i < rankings.size(); ++i) {
        const Ranking &entry = rankings[i];
        
        // 排名
        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(i + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 0, rankItem);
        
        // 用户名
        QTableWidgetItem *userItem = new QTableWidgetItem(entry.userId);
        userItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 1, userItem);
        
        // 用时（格式化为分:秒）
        QTableWidgetItem *timeItem = new QTableWidgetItem(formatTime(entry.penaltySeconds));
        timeItem->setTextAlignment(Qt::AlignCenter);
        rankingTable->setItem(i, 2, timeItem);
    }
}

QString RankPage::formatTime(int seconds) const
{
    int minutes = seconds / 60;
    int remainSeconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(remainSeconds, 2, 10, QChar('0'));
} 