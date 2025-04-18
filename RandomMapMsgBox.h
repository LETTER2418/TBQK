#include "Lbutton.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QLineEdit>
#include <QSlider>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QIntValidator>
#include <QSpinBox>

class RandomMapMsgBox : public QMessageBox
{
    Q_OBJECT

public:
    explicit RandomMapMsgBox(QMessageBox*parent = nullptr);
    ~RandomMapMsgBox();

    Lbutton *closeButton;
    int rings;

private:
    void paintEvent(QPaintEvent *event) override;
    QSpinBox *spinBox;
    QLabel *messageLabel;
    QPixmap backgroundImage;
    const QString backgroundImagePath=":\\image\\msg.png"; // 存储背景图片路径

};
