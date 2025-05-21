// MessageBox.cpp
#include "messagebox.h"
#include <QFileInfo>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>

MessageBox::MessageBox(QWidget *parent, bool showCancelButton)
    : QWidget(parent), messageLabel(new QLabel(this)), eventLoop(nullptr), dialogCode(0)
{
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->resize(300, 400);
    this->setFixedSize(300, 400);

    // 创建closeButton
    closeButton = new Lbutton(this, "确认");

    // 只有在showCancelButton为true时才创建取消按钮
    if (showCancelButton)
        {
            cancelButton = new Lbutton(this, "取消");

            // 当有两个按钮时，左右对称放置
            int centerX = width() / 2;
            int buttonY = 300;
            int buttonSpacing = 50; // 按钮之间的间距

            // 计算左右按钮的位置，使其对称
            cancelButton->move(centerX - closeButton->width() - buttonSpacing / 2, buttonY);
            closeButton->move(centerX + buttonSpacing / 2, buttonY);
        }
    else
        {
            // 只有一个按钮时，居中放置
            cancelButton = nullptr; // 不需要取消按钮时，将指针设为nullptr
            closeButton->move((width() - closeButton->width()) / 2, 300);
        }

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile())
        {
            backgroundImage.load(backgroundImagePath); // 如果文件存在，则加载图片
        }
    else
        {
            qWarning() << "Image file not found: " << backgroundImagePath;
        }

    // 设置消息标签
    messageLabel->setStyleSheet("QLabel { color: white; font-size: 20px; }");

    // 居中显示
    messageLabel->move((width() - messageLabel->width()) / 2, (height() - messageLabel->height()) / 2);

    // 默认隐藏窗口
    hide();
}

MessageBox::~MessageBox()
{
    if (eventLoop && eventLoop->isRunning())
        {
            eventLoop->exit(0);
            delete eventLoop;
        }
}

void MessageBox::setMessage(const QString &message)
{
    // 先设置文本
    messageLabel->setText(message);

    // 设置最大宽度为窗口宽度的78%
    int maxWidth = width() * 0.78;

    // 启用自动换行并设置固定宽度
    messageLabel->setWordWrap(true);
    messageLabel->setFixedWidth(maxWidth);

    // 调整大小以适应内容
    messageLabel->adjustSize();

    // 检查文本是否为单行
    if (!message.contains('\n') && messageLabel->height() <= messageLabel->fontMetrics().height() * 1.5)
        {
            // 单行文本：水平居中对齐
            messageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    else
        {
            // 多行文本：水平居中对齐
            messageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }

    // 重新居中消息标签在窗口中的位置
    int x = (width() - messageLabel->width()) / 2;
    int y = (height() - messageLabel->height()) / 2;
    messageLabel->move(x, y);
}

int MessageBox::exec()
{
    if (parentWidget())
        {
            // 获取父窗口所在的屏幕
            QScreen* parentScreen = parentWidget()->screen();
            if (parentScreen)
                {
                    // 获取父窗口在屏幕上的全局位置
                    QPoint parentGlobalPos = parentWidget()->mapToGlobal(QPoint(0, 0));
                    QRect parentRect = parentWidget()->geometry();

                    // 计算消息框在父窗口屏幕上的位置
                    int x = parentGlobalPos.x() + (parentRect.width() - this->width()) / 2;
                    int y = parentGlobalPos.y() + (parentRect.height() - this->height()) / 2;

                    // 确保消息框完全在父窗口所在的屏幕内
                    QRect screenGeometry = parentScreen->geometry();
                    x = qBound(screenGeometry.left(), x, screenGeometry.right() - this->width());
                    y = qBound(screenGeometry.top(), y, screenGeometry.bottom() - this->height());

                    this->move(x, y);
                }
            else
                {
                    // 如果无法获取父窗口的屏幕，则使用默认的居中逻辑
                    QRect parentRect = parentWidget()->geometry();
                    this->move(parentRect.x() + (parentRect.width() - this->width()) / 2,
                               parentRect.y() + (parentRect.height() - this->height()) / 2);
                }
        }
    else
        {
            // 如果没有父控件，则居中于主屏幕
            QScreen *screen = QGuiApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();
            this->move((screenGeometry.width() - this->width()) / 2,
                       (screenGeometry.height() - this->height()) / 2);
        }

    // 显示对话框
    show();
    raise();
    activateWindow();

    // 创建一个新的事件循环，确保不重复创建和调用
    if (eventLoop)
        {
            if (eventLoop->isRunning())
                {
                    // 如果事件循环已经在运行，直接返回上一次结果
                    qDebug() << "Event loop is already running, returning previous result";
                    return dialogCode;
                }
            delete eventLoop;
        }

    eventLoop = new QEventLoop();

    // 运行事件循环，直到调用accept()或reject()
    return eventLoop->exec();
}

void MessageBox::accept()
{
    // 设置对话框结果为1（类似QDialog::Accepted）并关闭事件循环
    dialogCode = 1;
    hide();
    if (eventLoop && eventLoop->isRunning())
        {
            eventLoop->exit(dialogCode);
        }
}

void MessageBox::reject()
{
    // 设置对话框结果为0（类似QDialog::Rejected）并关闭事件循环
    dialogCode = 0;
    hide();
    if (eventLoop && eventLoop->isRunning())
        {
            eventLoop->exit(dialogCode);
        }
}

void MessageBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull())
        {
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
    if (messageLabel)
        {
            int x = (width() - messageLabel->width()) / 2;
            int y = (height() - messageLabel->height()) / 2;
            messageLabel->move(x, y);
        }

    // 更新按钮位置，使其在窗口大小变化时保持对称
    int buttonY = 300;

    if (cancelButton)
        {
            // 有两个按钮时，左右对称放置
            int centerX = width() / 2;
            int buttonSpacing = 20;

            closeButton->move(centerX - closeButton->width() - buttonSpacing / 2, buttonY);
            cancelButton->move(centerX + buttonSpacing / 2, buttonY);
        }
    else if (closeButton)
        {
            // 只有一个按钮时，居中放置
            closeButton->move((width() - closeButton->width()) / 2, buttonY);
        }
}

