#include "mapmsgbox.h"
#include <QFileInfo>

MapMsgBox::MapMsgBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Test)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    //this->setWindowTitle("Custom Map Settings");  // 设置标题
    // 设置窗口的位置和大小 (x, y, width, height)
    this->setGeometry(700, 250, 300, 400);
    this->setStyleSheet("QLabel{min-width: 10px; min-height: 10px;}");

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile()) {
        backgroundImage.load(backgroundImagePath);
    } else {
        qWarning() << "Image file not found: " << backgroundImagePath;
    }

    ui->spinBox->setFixedSize(70, 70);
    rings = ui->spinBox->value() - 1;

    // 连接 ui->spinBox 的 valueChanged 信号到更新 rings 的槽
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) {
                rings = value - 1;  // 当 ui->spinBox 值改变时更新 rings
            });

    ui->color1Label->setStyleSheet("QLabel { background-color: black; }");
    color1 = Qt::black;
    connect(ui->color1Button, &QPushButton::clicked, this, &MapMsgBox::openColorDialog1);

    ui->color2Label->setStyleSheet("QLabel { background-color: white; }");
    color2 = Qt::white;
    connect(ui->color2Button, &QPushButton::clicked, this, &MapMsgBox::openColorDialog2);

    ui->color3Label->setStyleSheet("QLabel { background-color: green; }");
    color3 = Qt::green;
    connect(ui->color3Button, &QPushButton::clicked, this, &MapMsgBox::openColorDialog3);

    connect(ui->closeButton, &QPushButton::clicked, this, [this](){
        emit sendMsg(rings, color1, color2, color3);
        this->hide();
    });
}

MapMsgBox::~MapMsgBox()
{
    delete ui;
}

void MapMsgBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull()) {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}
