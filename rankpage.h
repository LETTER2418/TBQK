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

    void refreshRankingList(const QVector<Ranking>& rankings);
    Lbutton *backButton;

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void sortTable(int logicalIndex);
    void clearCurrentLevelData();

private:
    DataManager *dataManager;
    QTableWidget *rankingTable;
    QComboBox *levelSelector;
    QLabel *titleLabel;
    Lbutton *clearButton;
    
    int currentSortColumn = 2;
    Qt::SortOrder currentSortOrder = Qt::AscendingOrder;
    
    void setupUI();
    void connectSignals();
    QString formatTime(int seconds) const;
};

#endif // RANKPAGE_H 