#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include "lbutton.h"
#include "messagebox.h"
#include "mapdata.h"

class Game : public QWidget
{
    Q_OBJECT

public:
    Game(QWidget *parent = nullptr);
    ~Game();
    void setMap(MapData mapData);
    Lbutton *backButton;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void drawHexagon(QPainter &painter, const QPointF &center, int radius);

private:
    Lbutton *hintButton;
    Lbutton *withdrawButton;
    Lbutton *pathToggleButton;
    MessageBox *messageBox;
    QVector<QPoint>path;
    QVector<HexCell> hexagons;
    int radius = 50;
    QPointF center = QPointF(850, 440);
    QColor color1, color2, color3;
};

#endif // GAME_H
