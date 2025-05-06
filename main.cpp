#include <QApplication>
// #include "mainwindow.h"
#include "onlinemsgbox.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 注释原有代码
    // srand(time(0));
    // MainWindow mainWindow;
    // mainWindow.show();
    
    // 测试 OnlineMsgBox 类
    OnlineMsgBox *msgBox = new OnlineMsgBox();
    msgBox->show();
    
    return app.exec();
}
