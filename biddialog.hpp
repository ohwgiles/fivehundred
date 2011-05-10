#ifndef BIDDIALOG_HPP
#define BIDDIALOG_HPP

#include <QDialog>

#include "bid.hpp"

class Bidding;
class Human;

namespace Ui {
    class BidDialog;
}

class BidDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BidDialog(QWidget *parent = 0);
    ~BidDialog();

    void show(Human* player, Bidding* bids);

private:
    Ui::BidDialog *ui;
    Human* m_player;

signals:
    void bidMade(Human* player, Bid);

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void dialog_accepted();
    void dialog_rejected();
};

#endif // BIDDIALOG_HPP
