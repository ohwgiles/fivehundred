#include "game.hpp"

#include <sstream>

#include "deck.hpp"
#include "player.hpp"
#include "contract.hpp"
#include "bid.hpp"
#include "log.hpp"


void Game::updateScores(int northsouth, int eastwest) {
    info;
    m_scores.push_back(std::pair<int,int>(northsouth, eastwest));
    m_scores_sum.first += m_scores.back().first;
    m_scores_sum.second += m_scores.back().second;
}

void Game::abort() {
    trace;
    m_contracts.back()->abort();
    for(Player* p: m_players)
        p->abort();
}

Game::Game(Deck& deck) :
    QThread(),
    m_deck(deck),
    m_players({0,0,0,0}),
    m_num_players(0),
    m_first_player(0),
    m_scores_sum({0,0})
{
    trace;
}

Game::~Game() {
    trace;
    delete m_players[0];
    delete m_players[1];
    delete m_players[2];
    delete m_players[3];
    foreach(Contract* c, m_contracts)
        delete c;
}

void Game::addPlayer(Player* new_player) {
    trace;
    if(m_num_players >= 4) fatal(error<<"Already have 4 players");
    m_players[m_num_players++] = new_player;
}

void Game::run() {
    trace;

    if(m_num_players != 4)
        fatal(error<<"Tried to start without 4 players");

    for(int i=0; i<4; ++i) m_players[i]->next = m_players[(i+1)%4];
    for(int i=0; i<4; ++i) m_players[i]->configure();

    m_first_player = m_players[0];

    do {
        m_contracts.push_back(new Contract(m_first_player));
        emit newContract(m_contracts.back());

        info << "Dealing a new set of cards";
        m_deck.deal(m_players[0]->hand, m_players[1]->hand, m_players[2]->hand, m_players[3]->hand, m_contracts.back()->m_kitty);
        for(Player* p: m_players) {
            for(unsigned i=0; i< p->hand.size(); ++i) {
                p->hand[i]->setLocation(Card::HAND);
                p->hand[i]->setFromSeat(p->pos());
            }
        }
        for(Card* c: m_contracts.back()->m_kitty)
            c->setLocation(Card::HIDDEN);
        emit updateScene();

        Contract::ExitState state = m_contracts.back()->start();
        switch(state) {
        case Contract::REDEAL:
        case Contract::NOBIDS:
            error << "The contract was invalidated, restarting it";
            disconnect(m_contracts.back());
            delete m_contracts.back();
            m_contracts.pop_back();
            continue;
            break;
        case Contract::ABORT:
            info << "Aborting";
            return;
            break;
        case Contract::SUCCESS: {
            const Contract* c = m_contracts.back();
            const Bidding::Pair& p = c->bidWinner();
            if(p.player == m_players[SOUTH] || p.player == m_players[NORTH])
                updateScores((c->successful() ? 1 : -1) * p.bid.worth(), 10 * (c->tricksWon(m_players[EAST]) + c->tricksWon(m_players[WEST])));
            else
                updateScores(10 * (c->tricksWon(m_players[NORTH]) + c->tricksWon(m_players[SOUTH])), (c->successful() ? 1 : -1) * p.bid.worth());
            emit(updateNorthSouthScore(QString::number(m_scores_sum.first)));
            emit(updateEastWestScore(QString::number(m_scores_sum.second)));
            if(m_scores_sum.first >= 500 || m_scores_sum.first <= -500 || m_scores_sum.second >= 500 || m_scores_sum.second <= -500)
                return;
            disconnect(m_contracts.back());
            m_first_player = m_first_player->next;
        }
        break;
        }

    } while(true);
}
