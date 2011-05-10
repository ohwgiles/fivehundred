#include "scorechart.hpp"
#include "ui_scorechart.h"

#include "game.hpp"
#include "contract.hpp"

#include <sstream>
#include "log.hpp"
ScoreChart::ScoreChart(Game* g, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoreChart)
{
    ui->setupUi(this);
    ui->tableWidget->setRowCount(g->m_contracts.size());
    QStringList l;
    l << (g->m_players[0]->name + "/" + g->m_players[2]->name);
    l << (g->m_players[1]->name + "/" + g->m_players[3]->name);
    ui->tableWidget->setHorizontalHeaderLabels(l);
    int score_sum[2] = {0,0};
    for(unsigned i=0; i<g->m_contracts.size(); ++i) {
        Contract* c = g->m_contracts.at(i);
        score_sum[0] += g->m_scores.at(i).first;
        score_sum[1] += g->m_scores.at(i).second;
        std::stringstream ss;
        ss << c->bidWinner().bid << " by " << c->bidWinner().player->name;
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(score_sum[0])));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(score_sum[1])));
        ui->tableWidget->item(i,0)->setToolTip(ss.str().c_str());
        ui->tableWidget->item(i,1)->setToolTip(ss.str().c_str());
    }
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    show();
}

ScoreChart::~ScoreChart()
{
    delete ui;
}

void ScoreChart::on_dismissButton_clicked()
{
    this->close();
}
