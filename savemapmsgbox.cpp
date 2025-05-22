#include "savemapmsgbox.h"
#include <QLabel>
#include <QPixmap>
#include <QFont>
#include <QGuiApplication>
#include <QScreen>
#include <QShowEvent>

SaveMapMsgBox::SaveMapMsgBox(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setWindowModality(Qt::ApplicationModal);
    this->resize(300, 400);
    this->setStyleSheet("QLabel{min-width: 10px; min-height: 10px;} QComboBox { min-height: 30px; font-size: 12pt; } ");

    // 设置背景标签
    bgLabel = new QLabel(this);
    bgLabel->setPixmap(QPixmap(":/image/msg.png"));
    bgLabel->setScaledContents(true);
    bgLabel->setGeometry(0, 0, this->width(), this->height());
    bgLabel->lower();

    // 创建提示标签
    promptLabel = new QLabel("选择保存的关卡", this);
    QFont promptFont = promptLabel->font();
    promptFont.setPointSize(14); // 设置提示文字大小
    promptLabel->setFont(promptFont);
    promptLabel->setAlignment(Qt::AlignCenter);              // 文字居中对齐
    promptLabel->setStyleSheet("QLabel { color : white; }"); // 设置文字颜色为白色

    // Manually create ComboBox
    comboBox = new QComboBox(this);
    comboBox->setGeometry(50, 100, 200, 40); // Example position and size

    // 添加12个关卡到下拉框
    for (int i = 1; i <= 12; i++)
    {
        comboBox->addItem(QString("关卡 %1").arg(i), i);
    }

    // Create Close Button as Lbutton
    int buttonFontSize = 14; // Example font size for Lbutton
    closeButton = new Lbutton(this, "确认", "white", buttonFontSize);

    // Calculate centered positions (adjust vertical spacing as needed)
    int promptHeight = 30;
    int comboHeight = 40;
    int buttonHeight = BUTTON_HEIGHT; // Use Lbutton's height constant
    int verticalSpacing = 50;
    int totalHeight = promptHeight + comboHeight + buttonHeight + 2 * verticalSpacing;
    int startY = (this->height() - totalHeight) / 2;

    int promptWidth = 200;
    int comboWidth = 200;
    int buttonWidth = BUTTON_WIDTH; // Use Lbutton's width constant

    int promptX = (this->width() - promptWidth) / 2;
    int comboX = (this->width() - comboWidth) / 2;
    int buttonX = (this->width() - buttonWidth) / 2;

    int promptY = startY - 30;
    int comboY = promptY + promptHeight + verticalSpacing;
    int buttonY = comboY + comboHeight + verticalSpacing + 50; // Added 20px downward shift

    // Set Geometries
    promptLabel->setGeometry(promptX, promptY, promptWidth, promptHeight);
    comboBox->setGeometry(comboX, comboY, comboWidth, comboHeight);
    closeButton->setGeometry(buttonX, buttonY, buttonWidth, buttonHeight);

    connect(closeButton, &QPushButton::clicked, this, [this]()
            {
        // 获取当前选择的关卡ID
        int levelId = comboBox->currentData().toInt();
        emit sendMsg(levelId);
        this->hide(); });
}

SaveMapMsgBox::~SaveMapMsgBox()
{
    // Qt handles child widgets, manual deletion usually not needed unless using raw pointers without parent
}

void SaveMapMsgBox::showEvent(QShowEvent *event)
{
    if (parentWidget())
    {
        QRect parentRect = parentWidget()->geometry();
        this->move(parentRect.x() + (parentRect.width() - this->width()) / 2,
                   parentRect.y() + (parentRect.height() - this->height()) / 2);
    }
    else
    {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        this->move((screenGeometry.width() - this->width()) / 2,
                   (screenGeometry.height() - this->height()) / 2);
    }
    QWidget::showEvent(event);
}
