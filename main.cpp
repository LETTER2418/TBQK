#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QGridLayout>
#include <QFont>
#include <QScreen>

// 常量定义
const int BUTTON_WIDTH = 120;
const int BUTTON_HEIGHT = 50;
const int FONT_SIZE = 14;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 获取屏幕大小
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    QWidget window;
    window.setWindowTitle("填充背景图片");
    window.resize(screenWidth, screenHeight);

    // 创建 QLabel 用于显示背景图片
    QLabel *bgLabel = new QLabel(&window);
    QPixmap bgImage("D:/Image/Pictures/好看/bg4.png");

    // 检查图片是否加载成功
    if (bgImage.isNull()) {
        qDebug() << "图片加载失败，请检查路径！";
        return -1; // 退出程序
    }

    // 设置 QLabel 填充整个窗口
    bgLabel->setPixmap(bgImage);
    bgLabel->setScaledContents(true);
    bgLabel->setGeometry(0, 0, window.width(), window.height());

    // 按钮字体
    QFont buttonFont;
    buttonFont.setPointSize(FONT_SIZE);

    // 创建四个按钮
    QPushButton *startButton = new QPushButton("开始游戏");
    QPushButton *aboutButton = new QPushButton("关于");
    QPushButton *settingsButton = new QPushButton("设置");
    QPushButton *exitButton = new QPushButton("退出");

    // 自定义按钮的长宽和字体
    startButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    aboutButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    settingsButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    exitButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    startButton->setFont(buttonFont);
    aboutButton->setFont(buttonFont);
    settingsButton->setFont(buttonFont);
    exitButton->setFont(buttonFont);

    // 连接退出按钮的点击信号到窗口的关闭槽函数
    QObject::connect(exitButton, &QPushButton::clicked, &window, &QWidget::close);

    // 创建网格布局管理器
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(startButton, 0, 0);
    layout->addWidget(settingsButton, 0, 1);
    layout->addWidget(aboutButton, 1, 0);
    layout->addWidget(exitButton, 1, 1);

    // 创建一个 QWidget 作为中央部件
    QWidget *buttonWidget = new QWidget(&window);
    buttonWidget->setLayout(layout);

    // 计算按钮布局位置，使其在窗口内对称居中
    int buttonWidgetWidth = BUTTON_WIDTH * 2 + 40;
    int buttonWidgetHeight = BUTTON_HEIGHT * 2 + 40;
    int buttonWidgetX = (screenWidth - buttonWidgetWidth) / 2;
    int buttonWidgetY = (screenHeight - buttonWidgetHeight) / 2;
    buttonWidget->setGeometry(buttonWidgetX, buttonWidgetY, buttonWidgetWidth, buttonWidgetHeight);

    window.show();
    return app.exec();
}
