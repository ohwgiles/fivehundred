#include "gtest/gtest.h"
#include <QApplication>
#include "bidding.hpp"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	// we need a QApplication since Cards contain QPixmaps
	QApplication app(argc, argv);
	return RUN_ALL_TESTS();
}

TEST(Bid, Worth) {
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

TEST(Bid, ClosedMisere) {
	Bid closed_misere(Bid::CLOSED_MISERE);
	EXPECT_LT(Bid(Suit::NONE, 7), closed_misere);
	EXPECT_LT(closed_misere, Bid(Suit::SPADES, 8));
}

TEST(Bid, OpenMisere) {
	Bid open_misere(Bid::OPEN_MISERE);
	EXPECT_LT(Bid(Suit::NONE, 9), open_misere);
	EXPECT_LT(open_misere, Bid(Suit::SPADES, 10));
}
