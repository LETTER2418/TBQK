// MessageBox.h
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include<QMessageBox>

class MessageBox : public QMessageBox
{
    Q_OBJECT

public:
    explicit MessageBox(QMessageBox*parent = nullptr);
    ~MessageBox();

    void setMessage(const QString &message);

private:
    void paintEvent(QPaintEvent *event) override;

    QLabel *messageLabel;
    QVBoxLayout *layout;
    QPixmap backgroundImage;
    const QString backgroundImagePath=":\\image\\msg.png"; // 存储背景图片路径
};

#endif // MESSAGEBOX_H
