// #include <QApplication>
// #include "WindowManager.h"

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     srand(time(0));
//     WindowManager wmg;
//     wmg.show();
//     return app.exec();
// }

#include <QApplication>
#include "RandomMapMsgBox.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RandomMapMsgBox *x = new RandomMapMsgBox;
    x->show();
    return app.exec();
}


// #include <QApplication>
// #include <QLabel>
// #include <QWidget>

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);

//     // 创建一个主窗口
//     QWidget window;
//     window.setWindowTitle("Example");
//     window.resize(500, 500); // 设置窗口大小

//     // 创建 QLabel 并设置其父部件为 window
//     QLabel *color1Label = new QLabel();
//     color1Label->move(100,100);
//     color1Label->setFixedSize(100, 100);  // 设置标签大小
//     color1Label->setStyleSheet("background-color: red;"); // 为了更容易看到，设置背景色

//     // 显示窗口
//     window.show();

//     return app.exec();
// }

