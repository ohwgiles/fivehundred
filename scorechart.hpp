#ifndef SCORECHART_HPP
#define SCORECHART_HPP

#include <QDialog>

class Game;

namespace Ui {
    class ScoreChart;
}

class ScoreChart : public QDialog
{
    Q_OBJECT

public:
    explicit ScoreChart(Game* g, QWidget *parent = 0);
    ~ScoreChart();

private slots:
    void on_dismissButton_clicked();

private:
    Ui::ScoreChart *ui;
};

#endif // SCORECHART_HPP
