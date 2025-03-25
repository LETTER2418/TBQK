#include <QApplication>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QGridLayout>
#include <QFont>
#include <widget.h>
#include <QStackedWidget>
#include "button.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Widget window;





    window.show();
    return app.exec();
}
