#include "gtest/gtest.h"
#include "card.hpp"
#include "trick.hpp"
#include "log.hpp"
#include <QApplication>
#include "dummy_player.hpp"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	// we need a QApplication since Cards contain QPixmaps
	QApplication app(argc, argv);
	Card::createBackPixmap();
	Log::setLogLevel("DEBUG");
	return RUN_ALL_TESTS();
}
TEST(Trick, RelativeWorthInTrumps) {
	Trick::Comparator cmp(Suit::DIAMONDS, Suit::CLUBS, false);
	EXPECT_TRUE(cmp(Card(Suit::CLUBS, Card::FOUR), Card(Suit::CLUBS, Card::FIVE)));
	EXPECT_TRUE(cmp(Card(Suit::SPADES, Card::ACE), Card(Suit::CLUBS, Card::FIVE)));
	EXPECT_TRUE(cmp(Card(Suit::CLUBS, Card::ACE), Card(Suit::DIAMONDS, Card::FOUR)));
	EXPECT_TRUE(cmp(Card(Suit::CLUBS, Card::ACE), Card(Suit::DIAMONDS, Card::JOKER)));
}

TEST(Trick, RelativeWorthInNoTrumps) {
	Trick::Comparator cmp(Suit::NONE, Suit::CLUBS, false);
	EXPECT_TRUE(cmp(Card(Suit::DIAMONDS, Card::ACE), Card(Suit::CLUBS, Card::FOUR)));
	EXPECT_TRUE(cmp(Card(Suit::NONE, Card::JOKER), Card(Suit::CLUBS, Card::FOUR)));
	EXPECT_TRUE(cmp(Card(Suit::CLUBS, Card::FOUR), Card(Suit::CLUBS, Card::JOKER)));
}

