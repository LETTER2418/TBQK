#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "lbutton.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QDialog>
#include <QEventLoop>

class MessageBox : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBox(QWidget *parent = nullptr, bool showCancelButton = false);
    ~MessageBox();

    void setMessage(const QString &message);
    int exec(); // 添加exec方法模拟QMessageBox的功能
    void accept(); // 添加accept方法
    void reject(); // 添加reject方法

    Lbutton *closeButton;
    Lbutton *cancelButton;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *messageLabel;
    QPixmap backgroundImage;
    const QString backgroundImagePath = ":\\image\\msg.png"; // 存储背景图片路径
    QEventLoop *eventLoop; // 事件循环用于模拟模态对话框
    int dialogCode; // 对话框结果代码
};

#endif // MESSAGEBOX_H
