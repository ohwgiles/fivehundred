#include "gtest/gtest.h"
#include <QApplication>
#include "card.hpp"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	// we need a QApplication since Cards contain QPixmaps
	QApplication app(argc, argv);
	return RUN_ALL_TESTS();
}

TEST(Trumps, JokerIsAlwaysTrump) {
	Card c(Suit::NONE, Card::JOKER);
	EXPECT_TRUE(c.isTrump(Suit::SPADES));
	EXPECT_TRUE(c.isTrump(Suit::CLUBS));
	EXPECT_TRUE(c.isTrump(Suit::DIAMONDS));
	EXPECT_TRUE(c.isTrump(Suit::HEARTS));
	EXPECT_TRUE(c.isTrump(Suit::NONE));
}

TEST(Trumps, RightBower) {
	Card c(Suit::DIAMONDS, Card::JACK);
	EXPECT_EQ(c.suit(Suit::DIAMONDS), Suit::DIAMONDS);
	EXPECT_TRUE(c.isTrump(Suit::DIAMONDS));
	EXPECT_EQ(c.value(Suit::DIAMONDS), Card::RIGHT_BOWER);
}

TEST(Trumps, LeftBower) {
	Card c(Suit::SPADES, Card::JACK);
	EXPECT_EQ(c.suit(Suit::CLUBS), Suit::CLUBS);
	EXPECT_TRUE(c.isTrump(Suit::CLUBS));
	EXPECT_EQ(c.value(Suit::CLUBS), Card::LEFT_BOWER);
}
