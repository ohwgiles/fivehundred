#ifndef SCORECHART_HPP
#define SCORECHART_HPP
/*!
  \file scorechart.hpp
    Copyright 2011 Oliver Giles

    This file is part of Five Hundred.

    Five Hundred is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Five Hundred is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Five Hundred.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QDialog>

class Game;

namespace Ui {
    class ScoreChart;
}

/*!
  \class ScoreChart
  \brief Simple table widget for showing the current score

  This widget should be displayed at the end of each Contract, allowing
  any Human players to acknowlege the current status and begin the next
  Contract
*/
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
