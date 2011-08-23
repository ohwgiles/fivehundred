#ifndef JOKERSUITDIALOG_HPP
#define JOKERSUITDIALOG_HPP

#include <QDialog>
#include "suit.hpp"

class Player;

namespace Ui {
    class JokerSuitDialog;
}

class JokerSuitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JokerSuitDialog(Player * player, QWidget *parent = 0);
    ~JokerSuitDialog();
    Suit chosenSuit() { return m_desired_suit; }

private slots:
    void on_buttonCancel_clicked();
    void on_buttonSpades_clicked();
    void on_buttonClubs_clicked();
    void on_buttonDiamonds_clicked();
    void on_buttonHearts_clicked();

private:
    Ui::JokerSuitDialog *ui;
    Suit m_desired_suit;
};

#endif // JOKERSUITDIALOG_HPP
