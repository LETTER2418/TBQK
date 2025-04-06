// MessageBox.h
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>

class MessageBox : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBox(QWidget *parent = nullptr);
    ~MessageBox();

    void setMessage(const QString &message);
    void setBackgroundImage(); // 接受文件路径

private:
    void paintEvent(QPaintEvent *event) override;

    QLabel *messageLabel;
    QVBoxLayout *layout;
    QPixmap backgroundImage;
    const QString backgroundImagePath=":\\image\\bg.jpg"; // 存储背景图片路径
};

#endif // MESSAGEBOX_H
