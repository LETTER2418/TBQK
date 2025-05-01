#include "leveleditor.h"
#include <QBoxLayout>

LevelEditor::LevelEditor(QWidget *parent):QWidget(parent)
{
    backButton = new Lbutton(this, "返回");
    backButton -> move(0, 0);
    randomButton = new Lbutton(this, "随机生成");
    customButton = new Lbutton(this, "自定义生成");

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(randomButton, 0, 0);
    layout->addWidget(customButton, 1, 0);
    setLayout(layout);
}

LevelEditor::~LevelEditor()
{

}
