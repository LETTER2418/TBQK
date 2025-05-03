#include "levelmode.h"
#include "ui_levelmode.h"

LevelMode::LevelMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LevelMode)
{
    ui->setupUi(this);
}

LevelMode::~LevelMode()
{
    delete ui;
}
