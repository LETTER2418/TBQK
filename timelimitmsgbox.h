#ifndef TIMELIMITMSGBOX_H
#define TIMELIMITMSGBOX_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include "lbutton.h"
#include <QShowEvent>
#include <QScreen>
#include <QSpinBox>
#include <QGuiApplication>

class TimeLimitMsgBox : public QWidget
{
    Q_OBJECT
public:
    explicit TimeLimitMsgBox(QWidget *parent = nullptr);
    Lbutton *closeButton;
    int getSpinBoxValue();

protected:
    void showEvent(QShowEvent *event) override;

private:
    QLabel *promptLabel;
    QLabel *bgLabel;
    QSpinBox *spinBox;
};

#endif // TIMELIMITMSGBOX_H
