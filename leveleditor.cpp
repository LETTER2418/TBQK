#include "LevelEditor.h"
#include <QBoxLayout>

LevelEditor::LevelEditor(QWidget *parent):QWidget(parent)
{
    backButton = new Lbutton("返回", this);
    backButton -> move(0, 0);
    randomButton = new Lbutton("随机生成", this);
    customButton = new Lbutton("自定义生成", this);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(randomButton, 0, 0);
    layout->addWidget(customButton, 1, 0);
    setLayout(layout);
}

LevelEditor::~LevelEditor()
{

}
