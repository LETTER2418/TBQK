#include "about.h"

About::About(QWidget *parent): QWidget(parent)
{

    backButton = new Lbutton(this, "返回");
    backButton->move(0, 0);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(100, 100, 100, 100);
    mainLayout->setSpacing(15);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; }"
                              "QScrollArea > QWidget > QWidget { background: transparent; }"
                              "QScrollArea > QWidget > QScrollBar { background: transparent; }");
    scrollArea->viewport()->setStyleSheet("background-color: transparent;");

    contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("QWidget { background-color: transparent; }");
    contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->setSpacing(10);

    // --- 游戏标题和简介 ---
    titleLabel = new QLabel("<h1>欢迎来到提笔乾坤</h1>", contentWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    contentLayout->addWidget(titleLabel);

    introLabel = new QLabel(
        "<p>这是一款引人入胜的益智解谜游戏。你需要通过点击翻转棋盘上的六边形色块，"
        "最终使得每一个同心圆环上的所有色块颜色一致即可过关！</p>", contentWidget);
    introLabel->setWordWrap(true);
    contentLayout->addWidget(introLabel);

    contentLayout->addSpacing(10);

    // --- 游戏规则 ---
    rulesTitleLabel = new QLabel("<h2>游戏规则</h2>", contentWidget);
    rulesTitleLabel->setWordWrap(true);
    contentLayout->addWidget(rulesTitleLabel);

    rulesContentLabel = new QLabel(
        "<ul>"
        "<li><b>开始游戏：</b>通过主菜单选择\"关卡模式\"或\"联机模式\"开始。</li>"
        "<li><b>游戏目标：</b>点击六边形使其翻转颜色。目标是让棋盘上每一个同心圆环内的所有六边形都变成相同的颜色。</li>"
        "<li><b>基本操作：</b>"
        "<ul>"
        "<li>点击一个六边形来改变它自身及其直接相邻的六边形的颜色。(<em>请根据您的实际翻转逻辑核实和修改此条</em>)</li>"
        "<li>您的点击必须形成一条连续的路径。</li>"
        "</ul>"
        "</li>"
        "</ul>", contentWidget);
    rulesContentLabel->setWordWrap(true);
    contentLayout->addWidget(rulesContentLabel);

    contentLayout->addSpacing(10);

    // --- 特色玩法/提示 ---
    featuresTitleLabel = new QLabel("<h2>特色与提示</h2>", contentWidget);
    featuresTitleLabel->setWordWrap(true);
    contentLayout->addWidget(featuresTitleLabel);

    featuresContentLabel = new QLabel(
        "<ul>"
        "<li><b>提示 (<tt>提示</tt>按钮)：</b>当您没有思路时可以尝试，每次使用会增加30秒罚时，注意提示的路径不确保是最优路径。</li>"
        "<li><b>撤销 (<tt>撤销</tt>按钮)：</b>回退到上一步操作。</li>"
        "<li><b>重置 (<tt>重置</tt>按钮)：</b>撤销所有翻转的六边形，恢复初始状态。</li>"
        "<li><b>路径显示/隐藏：</b>切换是否在棋盘上显示您当前已走过的路径。</li>"
        "<li><b>半径调整：</b>您可以选择自动调整六边形大小以适应窗口，或手动设定固定大小。</li>"
        "</ul>", contentWidget);
    featuresContentLabel->setWordWrap(true);
    contentLayout->addWidget(featuresContentLabel);

    contentLayout->addSpacing(10);

    // --- 计分与排行 ---
    rankingTitleLabel = new QLabel("<h2>计分与排行</h2>", contentWidget);
    rankingTitleLabel->setWordWrap(true);
    contentLayout->addWidget(rankingTitleLabel);
    rankingContentLabel = new QLabel("<p>您的通关用时和所用步数将被记录。用时越短、步数越少，排名越高！</p>", contentWidget);
    rankingContentLabel->setWordWrap(true);
    contentLayout->addWidget(rankingContentLabel);

    contentLayout->addStretch();

    contentWidget->setLayout(contentLayout);
    scrollArea->setWidget(contentWidget);

    // --- 设置字体大小 ---
    contentWidget->setStyleSheet(
        "QLabel { "
        "    font-size: 20px; "
        "    line-height: 1.5; " /* 行高调整 */
        "    color: white; " /* 设置文字颜色为白色 */
        "}"
        "QLabel h1 { font-size: 22px; font-weight: bold; }" /* H1 标题样式 */
        "QLabel h2 { font-size: 18px; font-weight: bold; }" /* H2 标题样式 */
    );

    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);
}

About::~About()
{
    // Qt 的父子对象机制会自动处理 QWidget 子对象的内存释放，
}
