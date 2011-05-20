#include "gtest/gtest.h"
#include <QApplication>
#include "bid.hpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    // we need a QApplication since Cards contain QPixmaps
    QApplication app(argc, argv);
    return RUN_ALL_TESTS();
}

TEST(Bid, RelativeWorth) {
    Bid pass(Bid::PASS);
    Bid last = pass;
    for(int i=6; i <= 10; ++i) {
        Bid spades(Suit::SPADES, i);
        Bid clubs(Suit::CLUBS, i);
        Bid diamonds(Suit::DIAMONDS, i);
        Bid hearts(Suit::HEARTS, i);
        Bid notrumps(Suit::NONE, i);
        EXPECT_LT(last, spades);
        EXPECT_LT(spades, clubs);
        EXPECT_LT(clubs, diamonds);
        EXPECT_LT(diamonds, hearts);
        EXPECT_LT(hearts, notrumps);
        last = notrumps;
    }
}

TEST(Bid, ExactWorth) {
    EXPECT_EQ(180, Bid(Suit::DIAMONDS, 7).worth());
    EXPECT_EQ(40, Bid(Suit::SPADES, 6).worth());
    EXPECT_EQ(360, Bid(Suit::CLUBS, 9).worth());
    EXPECT_EQ(300, Bid(Suit::HEARTS, 8).worth());
    EXPECT_EQ(520, Bid(Suit::NONE, 10).worth());
}

TEST(Bid, ClosedMisere) {
    Bid closed_misere(Bid::CLOSED_MISERE);
    EXPECT_LT(Bid(Suit::NONE, 7), closed_misere);
    EXPECT_LT(closed_misere, Bid(Suit::SPADES, 8));
    EXPECT_EQ(230, closed_misere.worth());
}

TEST(Bid, OpenMisere) {
    Bid open_misere(Bid::OPEN_MISERE);
    EXPECT_LT(Bid(Suit::NONE, 9), open_misere);
    EXPECT_LT(open_misere, Bid(Suit::SPADES, 10));
    EXPECT_EQ(430, open_misere.worth());
}
