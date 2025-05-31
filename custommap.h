#ifndef CUSTOMMAP_H
#define CUSTOMMAP_H

#include "lbutton.h"
#include "mapdata.h"
#include <QWidget>
#include "messagebox.h"
#include <QSpinBox>

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
    void resizeEvent(QResizeEvent *event) override;

private:
    QVector<QPoint> path;
    MessageBox *messageBox;
    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440);
    QColor color1, color2, color3;
    int id;
    int rings = 3;

    bool isAutoRadius = false;
    Lbutton *radiusAdjustButton;
    QSpinBox *radiusSpinBox;
    void onRadiusAdjustButtonClicked();
    void onRadiusSpinBoxChanged(int value);
    void resetHexagons(const QVector<HexCell> &currentHexagons, int oldRadius, int newRadius);

    // 获取偏移量
    QPointF getOffset();

    double calculatePathScore(const QVector<int> &path, const QVector<QVector<int>> &graph);
    bool simulatedAnnealing(const QVector<QVector<int>> &graph, QVector<int> &bestPath);
};

#endif // CUSTOMMAP_H
