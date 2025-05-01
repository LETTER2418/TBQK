#include "savemapmsgbox.h"
#include "ui_savemapmsgbox.h"

SaveMapMsgBox::SaveMapMsgBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SaveMapMsgBox)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setGeometry(700, 250, 300, 400);// 设置窗口的位置和大小 (x, y, width, height)
    this->setStyleSheet("QLabel{min-width: 10px; min-height: 10px;}");
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setPixmap(QPixmap(":/image/msg.png"));
    bgLabel->setScaledContents(true);
    bgLabel->setGeometry(0, 0, this->width(), this->height());
    bgLabel->lower();  // 关键：把它放到所有控件下面

    connect(ui->closeButton, &QPushButton::clicked, this, [this](){
        emit sendMsg(ui->spinBox->value());
        this->hide();
    });
}

SaveMapMsgBox::~SaveMapMsgBox()
{
    delete ui;
}


