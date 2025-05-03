#ifndef SAVEMAPMSGBOX_H
#define SAVEMAPMSGBOX_H

#include <QWidget>

namespace Ui {
class SaveMapMsgBox;
}

class SaveMapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit SaveMapMsgBox(QWidget *parent = nullptr);
    ~SaveMapMsgBox();

signals:
    void sendMsg(int);  // 发送消息的信号

private:
    Ui::SaveMapMsgBox *ui;
};

#endif // SAVEMAPMSGBOX_H
