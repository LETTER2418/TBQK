// MessageBox.cpp
#include "MessageBox.h"
#include "button.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QFileInfo>

MessageBox::MessageBox(QWidget *parent)
    : QWidget(parent), messageLabel(new QLabel(this)), layout(new QVBoxLayout(this))
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 去掉窗口边框
    setAttribute(Qt::WA_TranslucentBackground); // 透明背景
    this->setFixedSize(300,400);

    messageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(messageLabel);

    button *closeButton = new button("确认", this);
    connect(closeButton, &button::clicked, this, &QWidget::close);
    layout->addWidget(closeButton);

    setLayout(layout);

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile()) {
        backgroundImage.load(backgroundImagePath); // 如果文件存在，则加载图片
    } else {
        qWarning() << "Image file not found: " << backgroundImagePath;
    }
}

MessageBox::~MessageBox() {}

void MessageBox::setMessage(const QString &message)
{
    messageLabel->setText(message);
}


void MessageBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull()) {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}
