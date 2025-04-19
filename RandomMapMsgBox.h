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
#include <QColorDialog>

class RandomMapMsgBox : public QWidget
{
    Q_OBJECT

public:
    explicit RandomMapMsgBox(QWidget*parent = nullptr);
    ~RandomMapMsgBox();
    Lbutton *closeButton;
    int rings;

private slots:
    void openColorDialog1() {
        QColorDialog *colorDlg = new QColorDialog(this);
        QColor color = colorDlg->getColor(QColor(255,255,255));
        if (color.isValid()) {
            color1Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        }
    }
    void openColorDialog2() {
        QColorDialog *colorDlg = new QColorDialog(this);
        QColor color = colorDlg->getColor(QColor(255,255,255));
        if (color.isValid()) {
            color2Label->setStyleSheet("QLabel { background-color: " + color.name() + "; }");
        }
    }

private:
    void paintEvent(QPaintEvent *event) override;
    QSpinBox *spinBox;
    QLabel *messageLabel;
    QPixmap backgroundImage;
    const QString backgroundImagePath=":\\image\\msg.png"; // 存储背景图片路径
    Lbutton *color1Button;
    Lbutton *color2Button;
    Lbutton *numButton;
    QLabel *color1Label;
    QLabel *color2Label;
};
