#ifndef CUSTOMMAP_H
#define CUSTOMMAP_H

#include "lbutton.h"
#include "mapdata.h"
#include <QWidget>

class CustomMap : public QWidget
{
    Q_OBJECT

public:
    CustomMap(QWidget *parent = nullptr);
    ~CustomMap();
    Lbutton *backButton;
    Lbutton *saveButton;
    void generateHexagons(int r = 3, QColor c1 = Qt::black, QColor c2 = Qt::white, QColor c3 = Qt::green);
    MapData getMapData();
    void setId(int id_);
    int solvePuzzle();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);

private:
    // 计算路径的得分（访问的边数）
    double calculatePathScore(const QVector<int> &path, const QVector<QVector<int>> &graph);
    // 模拟退火算法判断是否能一笔联通，并返回最佳路径
    bool simulatedAnnealing(const QVector<QVector<int>> &graph, QVector<int> &bestPath);

    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440);
    QColor color1, color2, color3;
    QVector<QPoint> path;
    int id;
    int rings;
};

#endif // CUSTOMMAP_H
