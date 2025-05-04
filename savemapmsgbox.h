#ifndef SAVEMAPMSGBOX_H
#define SAVEMAPMSGBOX_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include "lbutton.h" 

class SaveMapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit SaveMapMsgBox(QWidget *parent = nullptr);
    ~SaveMapMsgBox();

signals:
    void sendMsg(int);  // 发送消息的信号

private:
    QComboBox *comboBox;
    Lbutton *closeButton;  
    QLabel *promptLabel;  
    QLabel *bgLabel; 
};

#endif // SAVEMAPMSGBOX_H
