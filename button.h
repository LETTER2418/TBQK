#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPainter>
#include <QTimer>
#include <QLinearGradient>
#include <QFont>

const int BUTTON_WIDTH = 120;
const int BUTTON_HEIGHT = 50;
const int FONT_SIZE = 14;

class BUTTON : public QPushButton
{
    Q_OBJECT

public:
    // 构造函数，接收按钮文字和音频文件路径（可选）
    explicit BUTTON(const QString &text, QWidget *parent = nullptr, const QString &soundFilePath = "../../sound/BUTTON.ogg");

protected:
    // 重写paintEvent实现自定义绘制
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 更新渐变效果
    void updateGradient();

private:
    QMediaPlayer *player;     // 用于播放按钮点击音效
    QAudioOutput *audioOutput; // 音频输出
    QTimer *timer;             // 定时器，用于更新流光效果
    int gradientOffset;        // 流光偏移量
};

#endif // BUTTON_H
