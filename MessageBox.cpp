// MessageBox.cpp
#include "MessageBox.h"
#include "button.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QFileInfo>

MessageBox::MessageBox(QMessageBox *parent)
    : QMessageBox(parent), messageLabel(new QLabel(this)), layout(new QVBoxLayout(this))
{
    this->setStyleSheet("QLabel{min-width: 300px; min-height: 400px;}");
    //setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 去掉窗口边框
    this->setStandardButtons(QMessageBox::NoButton);

    messageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(messageLabel);

    class button *closeButton = new class button("确认", this);
    closeButton->move(150,300);
    connect(closeButton, &button::clicked, this, &QMessageBox::accept);
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
    messageLabel->setStyleSheet("QLabel { color: white; font-size: 20px; }");
    messageLabel->move(30, 15);
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


