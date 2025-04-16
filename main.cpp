// // #include <QApplication>
// // #include <WindowManager.h>

// // int main(int argc, char *argv[]) {
// //     QApplication app(argc, argv);

// //     WindowManager wmg;
// //     wmg.show();
// //     return app.exec();
// // }

// #include <QApplication>
// #include <QWidget>
// #include <QPainter>
// #include <QVector>
// #include <cstdlib>
// #include <ctime>

// class HexagonBoard : public QWidget {
// public:
//     HexagonBoard(QWidget *parent = nullptr) : QWidget(parent) {
//         setFixedSize(600, 600);
//         std::srand(std::time(nullptr));  // 随机种子，用于黑白颜色的随机分布
//     }

// protected:
//     void paintEvent(QPaintEvent *event) override {
//         Q_UNUSED(event);

//         QPainter painter(this);
//         painter.setRenderHint(QPainter::Antialiasing);

//         int radius = 30;  // 六边形的半径
//         int rows = 6;     // 六边形的行数

//         // 计算六边形的大小和位置
//         for (int row = 0; row < rows; ++row) {
//             for (int col = 0; col < rows; ++col) {
//                 // 计算六边形的中心点
//                 QPointF center(200+col * radius * 1.5, 200+row * radius * std::sqrt(3));
//                 if (col % 2 == 1) {
//                     center.setY(center.y() + radius * std::sqrt(3) / 2);  // 偶数行偏移
//                 }

//                 // 随机选择颜色
//                 QColor hexColor = (std::rand() % 2 == 0) ? Qt::black : Qt::white;
//                 painter.setBrush(hexColor);
//                 painter.setPen(Qt::gray);

//                 // 绘制六边形
//                 drawHexagon(painter, center, radius);
//             }
//         }
//     }

// private:
//     void drawHexagon(QPainter &painter, const QPointF &center, int radius) {
//         QPolygonF hexagon;
//         for (int i = 0; i < 6; ++i) {
//             double angle = M_PI / 3 * i;  // 60度
//             double x = center.x() + radius * cos(angle);
//             double y = center.y() + radius * sin(angle);
//             hexagon << QPointF(x, y);
//         }
//         painter.drawPolygon(hexagon);
//     }
// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     HexagonBoard board;
//     board.show();
//     return app.exec();
// }

#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QList>
#include <QtMath>

class HexGridWidget : public QWidget {
public:
    HexGridWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setBackgroundRole(QPalette::Dark);
        setAutoFillBackground(true);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int radius = 3; // 六边形盘面半径
        const double hexSize = 50.0; // 六边形边长

        // 生成所有六边形坐标
        QList<QPoint> hexCoords;
        for (int q = -radius; q <= radius; ++q) {
            for (int r = -radius; r <= radius; ++r) {
                int s = -q - r;
                if (std::max({std::abs(q), std::abs(r), std::abs(s)}) > radius)
                    continue;
                hexCoords.append(QPoint(q, r));
            }
        }

        // 生成随机颜色
        QVector<QColor> colors;
        for (int i = 0; i < hexCoords.size(); ++i) {
            colors.append(QRandomGenerator::global()->bounded(2) ? Qt::black : Qt::white);
        }

        // 绘制六边形
        foreach (const QPoint &coord, hexCoords) {
            int q = coord.x();
            int r = coord.y();

            // 转换为屏幕坐标（平顶六边形布局）
            double x = hexSize * 1.5 * q;
            double y = hexSize * qSqrt(3) * (r + q / 2.0);

            // 平移到窗口中心
            QPointF center(x + width()/2, y + height()/2);

            // 创建六边形路径
            QPainterPath path;
            for (int i = 0; i < 6; ++i) {
                double angle = qDegreesToRadians(60 * i + 30); // 平顶六边形起始角度30度
                double dx = hexSize * qCos(angle);
                double dy = hexSize * qSin(angle);
                QPointF point(center.x() + dx, center.y() + dy);
                if (i == 0) path.moveTo(point);
                else path.lineTo(point);
            }
            path.closeSubpath();

            // 填充颜色并绘制边框
            painter.fillPath(path, colors.takeFirst());
            painter.setPen(QPen(Qt::gray, 1));
            painter.drawPath(path);
        }
    }

    QSize sizeHint() const override {
        return QSize(800, 600); // 推荐窗口尺寸
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    HexGridWidget *hexWidget = new HexGridWidget();
    QHBoxLayout *layout = new QHBoxLayout(&window);
    layout->addWidget(hexWidget);
    window.setLayout(layout);
    window.resize(800, 600);
    window.show();

    return app.exec();
}
