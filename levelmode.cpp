#include "levelmode.h"
#include "ui_levelmode.h"

LevelMode::LevelMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LevelMode)

{
    ui->setupUi(this);
    backButton = new Lbutton(this,"返回");
    backButton->move(0,0);
    buttons[0] = ui->button1;
    buttons[1] = ui->button2;
    buttons[2] = ui->button3;
    buttons[3] = ui->button4;
    buttons[4] = ui->button5;
    buttons[5] = ui->button6;
    buttons[6] = ui->button7;
    buttons[7] = ui->button8;
    buttons[8] = ui->button9;
    buttons[9] = ui->button10;
    buttons[10] = ui->button11;
    buttons[11] = ui->button12;

}

LevelMode::~LevelMode()
{
    delete ui;
}
