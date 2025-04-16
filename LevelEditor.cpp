#include "LevelEditor.h"
#include <QBoxLayout>

LevelEditor::LevelEditor(QWidget *parent):QWidget(parent)
{
    backButton = new BUTTON("返回", this);
    backButton -> move(0, 0);
    randomButton = new BUTTON("随机生成", this);
    customButton = new BUTTON("自定义生成", this);

    // 创建按钮的网格布局
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(randomButton, 0, 0);
    layout->addWidget(customButton, 1, 0);
    setLayout(layout);
}

LevelEditor::~LevelEditor()
{

}
