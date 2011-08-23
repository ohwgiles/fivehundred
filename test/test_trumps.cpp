#include "gtest/gtest.h"
#include <QApplication>
#include "card.hpp"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	// we need a QApplication since Cards contain QPixmaps
	QApplication app(argc, argv);
	Card::createBackPixmap();
	return RUN_ALL_TESTS();
}

TEST(Trumps, JokerIsTrumpWhenPlayingTrumps) {
	Card c(Suit::NONE, Card::JOKER);
	EXPECT_TRUE(c.suit(Suit::SPADES) == Suit::SPADES);
	EXPECT_TRUE(c.suit(Suit::CLUBS) == Suit::CLUBS);
	EXPECT_TRUE(c.suit(Suit::DIAMONDS) == Suit::DIAMONDS);
	EXPECT_TRUE(c.suit(Suit::HEARTS) == Suit::HEARTS);
}

TEST(Trumps, RightBower) {
	Card c(Suit::DIAMONDS, Card::JACK);
	EXPECT_EQ(c.suit(Suit::DIAMONDS), Suit::DIAMONDS);
	EXPECT_EQ(c.value(Suit::DIAMONDS), Card::RIGHT_BOWER);
}

TEST(Trumps, LeftBower) {
	Card c(Suit::SPADES, Card::JACK);
	EXPECT_EQ(c.suit(Suit::CLUBS), Suit::CLUBS);
	EXPECT_EQ(c.value(Suit::CLUBS), Card::LEFT_BOWER);
}
