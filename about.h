#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QFont>
#include "lbutton.h"

class About : public QWidget
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr); // Use explicit for single-argument constructors
    ~About();

    Lbutton *backButton;

private:
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QVBoxLayout *contentLayout;
    QLabel *titleLabel;
    QLabel *introLabel;
    QLabel *rulesTitleLabel;
    QLabel *rulesContentLabel;
    QLabel *featuresTitleLabel;
    QLabel *featuresContentLabel;
    QLabel *rankingTitleLabel;
    QLabel *rankingContentLabel;
    QGroupBox *aboutGroupBox;
};

#endif // ABOUT_H
