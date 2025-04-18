#include "RandomMapMsgBox.h"
#include "Lbutton.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QFileInfo>

RandomMapMsgBox::RandomMapMsgBox(QMessageBox *parent)
    : QMessageBox(parent), messageLabel(new QLabel(this))
{
    this->setStyleSheet("QLabel{min-width: 300px; min-height: 400px;}");
    this->setStandardButtons(QMessageBox::NoButton);

    closeButton = new Lbutton("确认", this);
    closeButton->move(70,300);

    QFileInfo checkFile(backgroundImagePath);
    if (checkFile.exists() && checkFile.isFile()) {
        backgroundImage.load(backgroundImagePath);
    } else {
        qWarning() << "Image file not found: " << backgroundImagePath;
    }

    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setRange(0, 10);
    spinBox->setValue(3);
    spinBox->setSingleStep(1);
    spinBox->setFixedSize(70, 70);
    spinBox->setStyleSheet("QSpinBox { padding: 10px; font-size: 16px; }");
    spinBox->move(120, 150);

    connect(closeButton, &QPushButton::clicked, this, [this, spinBox]{
        this->accept();
        qDebug() << spinBox->value() ;
    });

}

RandomMapMsgBox::~RandomMapMsgBox() {}


void RandomMapMsgBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull()) {
        // 绘制背景图片
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    }

    QWidget::paintEvent(event);
}


