#include "RandomMapMsgBox.h"
#include "Lbutton.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QFileInfo>

RandomMapMsgBox::RandomMapMsgBox(QWidget *parent)
    : QWidget(parent), messageLabel(new QLabel(this))
{
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    //this->setWindowTitle("Random Map Settings");  // 设置标题
    // 设置窗口的位置和大小 (x, y, width, height)
    this->setGeometry(700, 250, 300, 400);
    //this->resize(300,400);
    this->setStyleSheet("QLabel{min-width: 10px; min-height: 10px;}");

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile()) {
        backgroundImage.load(backgroundImagePath);
    } else {
        qWarning() << "Image file not found: " << backgroundImagePath;
    }

    int x1 = 60;
    int y1 = 50;
    int y2 = 150;
    int y3 = 250;

    numButton = new Lbutton("选择半径", this);
    numButton->move(x1,y1);

    spinBox = new QSpinBox(this);
    spinBox->setRange(0, 10);
    spinBox->setValue(3);
    spinBox->setSingleStep(1);
    spinBox->setFixedSize(70, 70);
    spinBox->setStyleSheet("QSpinBox { padding: 10px; font-size: 16px; }");
    spinBox->move(x1+130, y1);

    closeButton = new Lbutton("确认", this);
    closeButton->move(x1,300);
    connect(closeButton, &QPushButton::clicked, this, [this]{
        this->close();
    });

    // 创建一个按钮，点击时打开颜色选择对话框
    color1Button = new Lbutton("选择颜色1", this);
    color1Button->move(x1,y2);
    color1Label = new QLabel(this);
    color1Label->setStyleSheet("QLabel { background-color: white; border: 1px solid black; }");
    color1Label->move(color1Button->x() + color1Button->width() + 12, color1Button->y() + 2);
    color1Label->setFixedSize(40, 40);  // 设置标签大小
    connect(color1Button, &QPushButton::clicked, this, &RandomMapMsgBox::openColorDialog1);

    color2Button = new Lbutton("选择颜色2", this);
    color2Button->move(x1,y3);
    color2Label = new QLabel(this);
    color2Label->setStyleSheet("QLabel { background-color: white; border: 1px solid black; }");
    color2Label->move(color2Button->x() + color2Button->width() + 12, color2Button->y() + 2);
    color2Label->setFixedSize(40, 40);  // 设置标签大小
    connect(color2Button, &QPushButton::clicked, this, &RandomMapMsgBox::openColorDialog2);

}

RandomMapMsgBox::~RandomMapMsgBox() {}


void RandomMapMsgBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull()) {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}


