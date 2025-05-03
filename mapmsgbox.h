#ifndef MAPMSGBOX_H
#define MAPMSGBOX_H

#include <QWidget>
#include <QColorDialog>
#include "ui_Test.h"

namespace Ui {
class Test;
}

class MapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit MapMsgBox(QWidget*parent = nullptr);
    ~MapMsgBox();
    int rings;
    QColor color1,color2,color3;

signals:
    void sendMsg(int, QColor, QColor, QColor);

private slots:
    void openColorDialog1() {
        QColor color = QColorDialog::getColor(QColor(255,255,255));
        if (color.isValid()) {
            ui->color1Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        }
        color1 = color;
    }
    void openColorDialog2() {
        QColor color = QColorDialog::getColor(QColor(255,255,255));
        if (color.isValid()) {
            ui->color2Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        }
        color2 = color;
    }
    void openColorDialog3() {
        QColor color = QColorDialog::getColor(QColor(255,255,255));
        if (color.isValid()) {
            ui->color3Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        }
        color3 = color;
    }

private:
    void paintEvent(QPaintEvent *event) override;
    QPixmap backgroundImage;
    const QString backgroundImagePath=":\\image\\msg.png"; // 存储背景图片路径
    Ui::Test *ui;
};

#endif // MAPMSGBOX_H
