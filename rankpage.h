#ifndef RANKPAGE_H
#define RANKPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "lbutton.h"
#include "datamanager.h"

class RankPage : public QWidget
{
    Q_OBJECT

public:
    explicit RankPage(QWidget *parent = nullptr, DataManager *dataManager = nullptr);
    ~RankPage();

    void refreshRankingList(int levelId);
    Lbutton *backButton;

private:
    DataManager *dataManager;
    QTableWidget *rankingTable;
    QComboBox *levelSelector;
    QLabel *titleLabel;
    
    void setupUI();
    void connectSignals();
    QString formatTime(int seconds) const;
};

#endif // RANKPAGE_H 