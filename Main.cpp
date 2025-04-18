// #include <QApplication>
// #include <WindowManager.h>

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     srand(time(0));
//     WindowManager wmg;
//     wmg.show();
//     return app.exec();
// }

// #include <QApplication>
// #include "RandomMapMsgBox.h"

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);
//     RandomMapMsgBox *x = new RandomMapMsgBox;
//     x->exec();
// }

#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QWidget>
#include <QVBoxLayout>

class ColorPicker : public QWidget {


public:
    ColorPicker(QWidget *parent = nullptr) : QWidget(parent) {
        // 创建一个按钮，点击时打开颜色选择对话框
        QPushButton *button = new QPushButton("选择颜色", this);
        connect(button, &QPushButton::clicked, this, &ColorPicker::openColorDialog);

        // 创建一个标签，用于显示所选颜色
        colorLabel = new QLabel(this);
        colorLabel->setFixedSize(100, 30);

        // 设置布局
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(button);
        layout->addWidget(colorLabel);
        setLayout(layout);
    }

private slots:
    void openColorDialog() {
        // 打开颜色选择对话框
        QColor color = QColorDialog::getColor(Qt::white, this, "选择颜色");
        if (color.isValid()) {
            // 更新标签以显示所选颜色
            colorLabel->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        }
    }

private:
    QLabel *colorLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ColorPicker window;
    window.setWindowTitle("颜色选择器");
    window.show();

    return app.exec();
}



