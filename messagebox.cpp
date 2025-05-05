// MessageBox.cpp
#include "messagebox.h"
#include "lbutton.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QFileInfo>

MessageBox::MessageBox(QMessageBox *parent, bool showCancelButton)
    : QMessageBox(parent), messageLabel(new QLabel(this))
{
    this->setStyleSheet("QLabel{min-width: 300px; min-height: 400px;}");
    //setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 去掉窗口边框
    this->setStandardButtons(QMessageBox::NoButton);

    closeButton = new Lbutton(this, "确认");
    
    // 只有在showCancelButton为true时才创建取消按钮
    if (showCancelButton) {
        cancelButton = new Lbutton(this, "取消");
        
        // 当有两个按钮时，左右对称放置
        int centerX = width() / 2;
        int buttonY = 300;
        int buttonSpacing = 50; // 按钮之间的间距
        
        // 计算左右按钮的位置，使其对称
        closeButton->move(centerX - closeButton->width() - buttonSpacing/2, buttonY);
        cancelButton->move(centerX + buttonSpacing/2, buttonY);
    } else {
        // 只有一个按钮时，居中放置
        cancelButton = nullptr; // 不需要取消按钮时，将指针设为nullptr
        closeButton->move((width() - closeButton->width()) / 2, 300);
    }

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
    messageLabel->setAlignment(Qt::AlignCenter);//文字居中
    messageLabel->setWordWrap(true); // 添加自动换行
    messageLabel->adjustSize(); // 调整标签大小以适应文本
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

void MessageBox::resizeEvent(QResizeEvent *event)
{
    // 调用父类的resizeEvent以保证正常行为
    QMessageBox::resizeEvent(event);

    // 居中设置 messageLabel
    if (messageLabel) {
        int x = (width() - messageLabel->width()) / 2;
        int y = (height() - messageLabel->height()) / 2;
        messageLabel->move(x, y);
    }
    
    // 更新按钮位置，使其在窗口大小变化时保持对称
    int buttonY = 300;
    
    if (cancelButton) {
        // 有两个按钮时，左右对称放置
        int centerX = width() / 2;
        int buttonSpacing = 50;
        
        closeButton->move(centerX - closeButton->width() - buttonSpacing/2, buttonY);
        cancelButton->move(centerX + buttonSpacing/2, buttonY);
    } else if (closeButton) {
        // 只有一个按钮时，居中放置
        closeButton->move((width() - closeButton->width()) / 2, buttonY);
    }
}
