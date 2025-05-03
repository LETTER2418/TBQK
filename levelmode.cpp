#include "levelmode.h"
#include "ui_levelmode.h"

LevelMode::LevelMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LevelMode)
{
    ui->setupUi(this);
    backButton = new Lbutton(this,"返回");
    backButton->move(0,0);
}

LevelMode::~LevelMode()
{
    delete ui;
}
