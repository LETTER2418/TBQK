#ifndef SAVEMAPMSGBOX_H
#define SAVEMAPMSGBOX_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include "lbutton.h"
#include <QShowEvent>
#include <QScreen>
#include <QGuiApplication>

class SaveMapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit SaveMapMsgBox(QWidget *parent = nullptr);
    ~SaveMapMsgBox();

signals:
    void sendMsg(int);  // 发送消息的信号

protected:
    void showEvent(QShowEvent *event) override;

private:
    QComboBox *comboBox;
    Lbutton *closeButton;  
    QLabel *promptLabel;  
    QLabel *bgLabel; 
};

#endif // SAVEMAPMSGBOX_H
