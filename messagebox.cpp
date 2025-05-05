// MessageBox.cpp
#include "messagebox.h"
#include <QFileInfo>

MessageBox::MessageBox(QWidget *parent, bool showCancelButton)
    : QWidget(parent), messageLabel(new QLabel(this)), eventLoop(nullptr), dialogCode(0)
{
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setGeometry(700, 250, 300, 400);

    // 创建closeButton
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
    
    // 设置消息标签
    messageLabel->setStyleSheet("QLabel { color: white; font-size: 20px; }");
    messageLabel->setAlignment(Qt::AlignCenter); // 文字居中
    messageLabel->setWordWrap(true); // 添加自动换行
    
    // 居中显示
    messageLabel->move((width() - messageLabel->width()) / 2, (height() - messageLabel->height()) / 2);

    // 默认隐藏窗口
    hide();
}

MessageBox::~MessageBox() 
{
    if (eventLoop && eventLoop->isRunning()) {
        eventLoop->exit(0);
        delete eventLoop;
    }
}

void MessageBox::setMessage(const QString &message)
{
    messageLabel->setText(message);
    messageLabel->adjustSize(); // 调整标签大小以适应文本
    
    // 重新居中消息标签
    int x = (width() - messageLabel->width()) / 2;
    int y = (height() - messageLabel->height()) / 2;
    messageLabel->move(x, y);
}

int MessageBox::exec()
{
    // 显示对话框
    show();
    raise();
    activateWindow();

    // 创建一个新的事件循环
    if (!eventLoop) {
        eventLoop = new QEventLoop();
    }
    
    // 运行事件循环，直到调用accept()或reject()
    return eventLoop->exec();
}

void MessageBox::accept()
{
    // 设置对话框结果为1（类似QDialog::Accepted）并关闭事件循环
    dialogCode = 1;
    hide();
    if (eventLoop && eventLoop->isRunning()) {
        eventLoop->exit(dialogCode);
    }
}

void MessageBox::reject()
{
    // 设置对话框结果为0（类似QDialog::Rejected）并关闭事件循环
    dialogCode = 0;
    hide();
    if (eventLoop && eventLoop->isRunning()) {
        eventLoop->exit(dialogCode);
    }
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
    QWidget::resizeEvent(event);

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

 