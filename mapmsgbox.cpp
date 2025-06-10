#include "mapmsgbox.h"
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QGuiApplication>
#include <QScreen>

MapMsgBox::MapMsgBox(QWidget *parent)
    : QWidget(parent), hasUsedFadeAnimation(false)
{
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setWindowModality(Qt::ApplicationModal);
    this->resize(300, 400);
    this->setStyleSheet("QLabel{min-width: 10px; min-height: 10px;}");

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile())
    {
        backgroundImage.load(backgroundImagePath);
    }
    else
    {
        qWarning() << "Image file not found: " << backgroundImagePath;
    }

    setupUI();
    setupFadeAnimation(); // 初始化透明度动画

    color1 = Qt::black;
    color2 = Qt::white;
    color3 = Qt::green;

    // 设置初始颜色
    color1Label->setStyleSheet("QLabel { background-color: black; }");
    color2Label->setStyleSheet("QLabel { background-color: white; }");
    color3Label->setStyleSheet("QLabel { background-color: green; }");

    // 连接信号和槽
    connect(color1Button, &QPushButton::clicked, this, &MapMsgBox::openColorDialog1);
    connect(color2Button, &QPushButton::clicked, this, &MapMsgBox::openColorDialog2);
    connect(color3Button, &QPushButton::clicked, this, &MapMsgBox::openColorDialog3);
    connect(closeButton, &QPushButton::clicked, this, [this]()
            {
        emit sendMsg(rings, color1, color2, color3);
        this->hide(); });

    // 设置comboBox和连接其信号
    rings = comboBox->currentData().toInt() - 1;
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            { rings = comboBox->itemData(index).toInt() - 1; });
}

void MapMsgBox::setupFadeAnimation()
{
    // 创建透明度效果
    opacityEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0.0); // 初始设置为完全透明

    // 创建动画
    fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeAnimation->setDuration(300); // 动画持续300毫秒
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void MapMsgBox::show()
{
    // 只在第一次调用时使用动画
    if (!hasUsedFadeAnimation)
    {
        opacityEffect->setOpacity(0.0);
        QWidget::show();
        fadeAnimation->start();
        hasUsedFadeAnimation = true;
    }
    else
    {
        // 直接显示窗口，不使用动画
        opacityEffect->setOpacity(1.0);
        QWidget::show();
    }
}

MapMsgBox::~MapMsgBox()
{
    // 不需要手动删除控件，Qt的父子对象系统会处理
}

void MapMsgBox::setupUI()
{
    // 设置全局字体
    QFont font;
    font.setPointSize(10);
    this->setFont(font);

    // 创建按钮 - 使用带颜色参数的构造函数
    int fontSize = 13;
    radiusButton = new Lbutton(this, "📏 选择环数", "white", fontSize);
    color1Button = new Lbutton(this, "🎨 选择六边形正色", "white", fontSize);
    color2Button = new Lbutton(this, "🖌️ 选择六边形反色", "white", fontSize);
    color3Button = new Lbutton(this, "🔍 选择路径颜色", "white", fontSize);
    closeButton = new Lbutton(this, "✅ 确认", "white", fontSize);

    // 设置按钮位置
    int buttonPositionX = 50;
    radiusButton->move(buttonPositionX, 20);
    color1Button->move(buttonPositionX, 100);
    color2Button->move(buttonPositionX, 180);
    color3Button->move(buttonPositionX, 250);
    closeButton->move(buttonPositionX, 320);

    // 创建标签
    color1Label = new QLabel(this);
    color2Label = new QLabel(this);
    color3Label = new QLabel(this);

    int labelSize = 41;
    color1Label->setFixedSize(labelSize, labelSize);
    color2Label->setFixedSize(labelSize, labelSize);
    color3Label->setFixedSize(labelSize, labelSize);

    // 调整标签位置，确保与按钮不重叠
    int labelPositionX = 220;
    color1Label->move(labelPositionX, 100);
    color2Label->move(labelPositionX, 180);
    color3Label->move(labelPositionX, 250);

    // 创建ComboBox
    comboBox = new QComboBox(this);
    comboBox->setFixedSize(80, 40);

    // 设置字体大小
    QFont comboFont;
    comboFont.setPointSize(12);
    comboBox->setFont(comboFont);

    comboBox->setStyleSheet("QComboBox { alternate-background-color: #f0f0f0; }");

    // 添加选项1-5
    for (int i = 1; i <= 5; i++)
    {
        comboBox->addItem(QString::number(i), i);
    }

    // 设置初始值
    comboBox->setCurrentIndex(4);
    comboBox->move(200, 30);
}

void MapMsgBox::openColorDialog1()
{
    QColor color = QColorDialog::getColor(QColor(255, 255, 255));
    if (color.isValid())
    {
        color1Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        color1 = color;
    }
}

void MapMsgBox::openColorDialog2()
{
    QColor color = QColorDialog::getColor(QColor(255, 255, 255));
    if (color.isValid())
    {
        color2Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        color2 = color;
    }
}

void MapMsgBox::openColorDialog3()
{
    QColor color = QColorDialog::getColor(QColor(255, 255, 255));
    if (color.isValid())
    {
        color3Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        color3 = color;
    }
}

void MapMsgBox::showEvent(QShowEvent *event)
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

void MapMsgBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull())
    {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}
