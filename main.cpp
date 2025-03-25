#include <QApplication>
#include <start.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    start main;
    main.show();
    return app.exec();
}
