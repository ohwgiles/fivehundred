#ifndef MainWindow_HPP
#define MainWindow_HPP

#include <QMainWindow>

#include <QLabel>

#include <QGraphicsTextItem>

#include "seat.hpp"

namespace Ui {
    class MainWindow;
}

class QGraphicsScene;
class Deck;
class BidDialog;
class QAbstractListModel;
class QPushButton;
class Human;
class Card;
class Bidding;
class Contract;
class Game;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool open_hand, QWidget *parent = 0);
    virtual ~MainWindow();

    Deck& deck() const { return *m_deck; }
    BidDialog& biddialog() const { return *m_biddialog; }
    QPushButton& kittybutton() const { return *m_kittybutton; }

protected:
    void resizeEvent(QResizeEvent* event);
    void resetUI();

signals:
    void reposition();
    void animationComplete();

protected slots:
    void updateDisplay();
    void showBidDialog(Human* player, Bidding* bids);
    void showKittyButton(bool);
    void enableKittyButton(bool);
    void animatePlayCard(Card*);
    void animateCollectCards(std::vector<Card*>, Seat);
    void animationComplete_();
    void threadFinished();
    void connectContract(Contract* contract);

private slots:
    void on_actionNew_Game_triggered();

    void on_actionEnd_Game_triggered();

    void on_actionAbout_triggered();

private:
    void newGame();

    Ui::MainWindow *ui;

    Game* m_game;

    QGraphicsScene* m_scene;
    Deck* m_deck;
    BidDialog* m_biddialog;
    QPushButton* m_kittybutton;
    QGraphicsProxyWidget* m_kittybuttonproxy;

    bool m_open_hand;

    enum Action { NONE, NEW_GAME, SHOW_SCORES };
    Action m_next_action;

    QLabel m_lbl_status;
    QLabel m_lbl_team_ew;
    QLabel m_lbl_team_ns;
    QLabel m_lbl_tricks_ew;
    QLabel m_lbl_tricks_ns;
    QLabel m_lbl_score_ew;
    QLabel m_lbl_score_ns;
    QLabel m_lbl_bid;

    QGraphicsTextItem m_player_names[4];

    //MainWindow* m_game;
};


#endif // MainWindow_HPP
