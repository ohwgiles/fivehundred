#include "gtest/gtest.h"
#include <QApplication>
#include "bidding.hpp"
#include "dummy_player.hpp"
#include <cstdlib>

DummyPlayer players[4];

void connectPlayers(Player* p) {
    p[0].next = &p[1];
    p[1].next = &p[2];
    p[2].next = &p[3];
    p[3].next = &p[0];
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    // we need a QApplication since Cards contain QPixmaps
    QApplication app(argc, argv);
    connectPlayers(players);
    return RUN_ALL_TESTS();
}

TEST(Bidding, Sequence) {
    Bidding b;
    int j = 0;
    for(int i=6; i <= 10; ++i) {
        for(Suit s : {Suit::SPADES, Suit::CLUBS, Suit::DIAMONDS, Suit::HEARTS, Suit::NONE}) {
            b.bid(&players[(++j)%4], Bid(s, i));
        }
    }
    EXPECT_FALSE(b.complete());
    EXPECT_TRUE(b.hasWinner());
}

TEST(Bidding, MaxBid) {
    Bidding b;
    EXPECT_FALSE(b.complete());
    EXPECT_FALSE(b.hasWinner());
    b.bid(&players[0], Bid(Suit::NONE, 10));
    EXPECT_EQ(&players[0], b.nextBidder(&players[0]));
    EXPECT_FALSE(b.complete());
    EXPECT_TRUE(b.hasWinner());
    b.bid(&players[0], Bid());
    EXPECT_TRUE(b.complete());
    EXPECT_TRUE(b.hasWinner());
}

TEST(Bidding, BidUpDifferentSuit) {
    Bidding b;
    Player* p = &players[0];
    b.bid(p, Bid(Suit::DIAMONDS, 6));
    // everyone else pass
    p = b.nextBidder(p);
    b.bid(p, Bid());
    p = b.nextBidder(p);
    b.bid(p, Bid());
    p = b.nextBidder(p);
    b.bid(p, Bid());
    p = b.nextBidder(p);
    EXPECT_EQ(p, &players[0]);
    b.bid(p, Bid(Suit::CLUBS, 7));
    EXPECT_EQ(b.nextBidder(p), &players[1]);
}

TEST(Bidding, AllPassed) {
    Bidding b;
    b.bid(&players[0], Bid());
    b.bid(&players[1], Bid());
    b.bid(&players[2], Bid());
    b.bid(&players[3], Bid());
    EXPECT_TRUE(b.complete());
    EXPECT_FALSE(b.hasWinner());
}
