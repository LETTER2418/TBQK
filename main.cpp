#include <QApplication>
#include <WindowManager.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    srand(time(0));
    WindowManager wmg;
    wmg.show();
    return app.exec();
}
