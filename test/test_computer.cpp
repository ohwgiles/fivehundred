#include "gtest/gtest.h"
#include <QApplication>
#include "bidding.hpp"
#include "computer.hpp"
#include "deck.hpp"
#include "trick.hpp"

#include <fstream>
#include <lua.hpp>
#include "log.hpp"

class ComputerFixture : public ::testing::Test, protected Computer {
public:
	ComputerFixture() :
		Computer(Player::SOUTH, "Jed")
   	{
	}

	void SetUp() {
		next = this;
        ASSERT_NO_THROW(configure());
	}

        void Deal() {
			std::vector<Card*> unused;
            m_deck.deal(hand, unused, unused, unused, m_kitty);
            ASSERT_NO_THROW(handDealt());
        }

	Deck m_deck;
	std::vector<Card*> m_kitty;
};

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	// we need a QApplication since Cards contain QPixmaps
	QApplication app(argc, argv);
        return RUN_ALL_TESTS();
}

TEST_F(ComputerFixture, yourTurnToBid) {
        Deal();
	Bidding bidding;
        EXPECT_EQ(bidding.count(), 0);
	EXPECT_EQ(lua_gettop(L), 0);
	Bid b;
        ASSERT_NO_THROW(b = yourTurnToBid(&bidding));
	EXPECT_EQ(lua_gettop(L), 0);
	debug << b;
}

TEST_F(ComputerFixture, bidWon) {
        Deal();
	Bidding bidding;
        ASSERT_NO_THROW(bidding.bid(this, Bid(Suit::CLUBS, 10)));
	EXPECT_EQ(lua_gettop(L), 0);

        ASSERT_NO_THROW(bidWon(&bidding, this));
	EXPECT_EQ(lua_gettop(L), 0);
}

TEST_F(ComputerFixture, yourTurnToSelectKitty) {
        Deal();
	EXPECT_EQ(lua_gettop(L), 0);
	debug;
        ASSERT_NO_THROW(yourTurnToSelectKitty(m_kitty));
	EXPECT_EQ(lua_gettop(L), 0);
}

TEST_F(ComputerFixture, yourTurnToPlay) {
        Deal();
        EXPECT_EQ(lua_gettop(L), 0);
        debug;
        Trick t;
        EXPECT_EQ(t.size(), 0);
		Card* c;
        ASSERT_NO_THROW(c = yourTurnToPlay(&t));
        EXPECT_EQ(lua_gettop(L), 0);
		debug << *c;
}

TEST_F(ComputerFixture, trickWon) {
        Deal();
        EXPECT_EQ(lua_gettop(L), 0);
        debug;
        Trick t;
        ASSERT_NO_THROW(t.playCard(this, hand.at(0)));
        ASSERT_NO_THROW(t.playCard(this, hand.at(1)));
        ASSERT_NO_THROW(t.playCard(this, hand.at(2)));
        ASSERT_NO_THROW(t.playCard(this, hand.at(3)));
        EXPECT_EQ(t.size(), 4);
        ASSERT_NO_THROW(trickWon(t, t.winner(Suit::DIAMONDS)));
        EXPECT_EQ(lua_gettop(L), 0);
}

