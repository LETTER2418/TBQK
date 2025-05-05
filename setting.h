#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include "lbutton.h"
#include "datamanager.h"
#include "messagebox.h"

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr, DataManager *dataManager = nullptr);
    ~Setting();

    Lbutton *backButton;    // 返回按钮
    Lbutton *clearDataButton; // 清除数据按钮

private:
    DataManager *dataManager;
    MessageBox *confirmMessageBox;  // 确认对话框
    MessageBox *successMessageBox;  // 成功提示对话框
};

#endif // SETTING_H
