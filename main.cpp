#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    srand(time(0));
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
