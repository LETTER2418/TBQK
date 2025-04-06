#include <QApplication>
#include <WindowManager.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WindowManager wmg;
    wmg.show();
    return app.exec();
}


