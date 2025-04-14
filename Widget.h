#ifndef WIDGET_H
#define WIDGET_H

#include <QStackedWidget>
#include <QLabel>
#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QGridLayout>
#include <QFont>
#include <Qwidget>
#include "BUTTON.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void startGame();
    ~Widget();

private:
    Ui::Widget *ui;

};
#endif // WIDGET_H
