
-- Test that even with the same number of cards (without aces), the relative values are being considered
HAND = {
    Card(8, "Clubs"), Card(9, "Clubs"), Card(10, "Clubs"), Card(Card.KING, "Clubs"), Card(Card.QUEEN, "Clubs"),
    Card(4, "Hearts"), Card(5, "Hearts"), Card(6, "Hearts"), Card(7, "Hearts"), Card(8, "Hearts")
}
handDealt()
expect_gt(CONFIDENCE["Clubs"], CONFIDENCE["Hearts"])

-- Test that the first bid will be 6 clubs
local b = yourTurnToBid({})
print(b)
expect_eq(b, Bid(6, "Clubs"))

-- Test that if another player bids we will outbid them
expect_eq(yourTurnToBid({ { player=LEFT, bid=Bid(7, "Diamonds") } }), Bid(8, "Clubs"))

-- Test that if the bid is too high we pass
expect_eq(yourTurnToBid({ { player=RIGHT, bid=Bid(10, "Diamonds") } }), Bid(Bid.PASS))

-- Let's say we win this one
bidWon({ { player=ME, bid=Bid(8, "Clubs") }}, me)

-- We should chuck out the kitty as we got it
local discards = yourTurnToSelectKitty({Card(5, "Spades"), Card(6, "Spades"), Card(7, "Spades")})
expect_true(table.contains(discards, Card(5, "Spades")))
expect_true(table.contains(discards, Card(6, "Spades")))
expect_true(table.contains(discards, Card(7, "Spades")))

-- Test a misere hand
HAND = {
    Card(5, "Clubs"), Card(6, "Clubs"), Card(7, "Clubs"),
    Card(4, "Diamonds"), Card(5, "Diamonds"), Card(6, "Diamonds"),
    Card(4, "Hearts"), Card(5, "Hearts"), Card(6, "Hearts"), Card(7, "Hearts")
}
handDealt()
-- this should be worth open misere
expect_eq(yourTurnToBid({ { player=RIGHT, bid=Bid(8, "Hearts") } }), Bid(Bid.OPEN_MISERE))
-- see what happens if we swap out some cards
table.delete(HAND, Card(5, "Clubs"))
table.insert(HAND, Card(Card.ACE, "Clubs"))
handDealt()
table.delete(HAND, Card(4, "Diamonds"))
table.insert(HAND, Card(Card.JOKER))
handDealt()
table.delete(HAND, Card(7, "Hearts"))
table.insert(HAND, Card(6, "Spades"))
handDealt()
-- this is still good enough for a closed misere bid
expect_eq(yourTurnToBid({ { player=RIGHT, bid=Bid(7, "Hearts") } }), Bid(Bid.CLOSED_MISERE))
-- but not open
expect_eq(yourTurnToBid({ { player=RIGHT, bid=Bid(8, "Hearts") } }), Bid(Bid.PASS))
-- let's say we win with closed misere, what would we throw out?
bidWon({ { player=RIGHT, bid=Bid(6, "Hearts") }, { player=ME, bid=Bid(Bid.CLOSED_MISERE) } }, ME)
-- good kitty
local discards = yourTurnToSelectKitty({Card(4, "Spades"), Card(5, "Spades"), Card(7, "Diamonds")})
expect_true(table.contains(discards, Card(Card.JOKER)))
expect_true(table.contains(discards, Card(Card.ACE, "Clubs")))

-- bad kitty
HAND = {
    Card(5, "Clubs"), Card(6, "Clubs"), Card(7, "Clubs"),
    Card(Card.JOKER), Card(5, "Diamonds"), Card(6, "Diamonds"),
    Card(4, "Hearts"), Card(5, "Hearts"), Card(6, "Hearts"), Card(7, "Hearts")
}
local discards = yourTurnToSelectKitty({Card(8, "Spades"), Card(10, "Clubs"), Card(9, "Spades")})
expect_true(table.contains(discards, Card(Card.JOKER)))
expect_true(table.contains(discards, Card(9, "Spades")))
expect_true(table.contains(discards, Card(8, "Spades")))


-- Test a kickass suited hand
HAND = {
    Card(4, "Hearts"), Card(5, "Hearts"), Card(6, "Hearts"), Card(Card.JACK, "Hearts"), Card(Card.JACK, "Diamonds"), Card(Card.ACE, "Hearts"), Card("Joker"),
    Card(Card.ACE, "Clubs"), Card(Card.ACE, "Spades"), Card(Card.ACE, "Diamonds"),
}
handDealt()
expect_eq(CONFIDENCE["Hearts"], 10)
expect_eq(CONFIDENCE["Clubs"], CONFIDENCE["Spades"])
expect_lt(CONFIDENCE["Clubs"], CONFIDENCE["Diamonds"])
expect_lt(CONFIDENCE["Diamonds"], CONFIDENCE["Hearts"])

-- Test that I remember my partner's and enemies' strengths
expect_eq(yourTurnToBid({ { player=LEFT, bid=Bid(7, "Spades") }, { player=PARTNER, bid=Bid(10, "Clubs") } }), Bid(10, "Hearts"))
expect_eq(PARTNER_STRENGTH, "Clubs")
expect_eq(ENEMY_STRENGTH, "Spades")

-- Test we recognise which suits are trumps
bidWon({{ player=ME, bid=Bid(8, "Hearts") }}, me)
expect_eq(TRUMP_SUIT, "Hearts")
expect_false(table.contains(NONTRUMPSUITS, "Hearts"))

bidWon({{ player=LEFT, bid=Bid(6, "Spades") }}, LEFT)
expect_eq(TRUMP_SUIT, "Spades")
expect_false(table.contains(NONTRUMPSUITS, "Spades"))
expect_true(table.contains(NONTRUMPSUITS, "Hearts"))

-- Test a kickass no trumps hand but no joker
HAND = {
    Card(Card.ACE, "Hearts"), Card(Card.KING, "Hearts"), Card(Card.QUEEN, "Hearts"),
    Card(Card.ACE, "Diamonds"), Card(Card.KING, "Diamonds"), Card(Card.QUEEN, "Diamonds"),
    Card(Card.ACE, "Clubs"), Card(Card.ACE, "Spades"), Card(Card.KING, "Spades")
}
handDealt()
expect_lt(CONFIDENCE["No Trumps"], CONFIDENCE["Diamonds"])
expect_lt(CONFIDENCE["No Trumps"], CONFIDENCE["Hearts"])

-- We pick up a perfect kitty, nothing to discard but just the lowest value of each suit
local discards = yourTurnToSelectKitty({Card(Card.JOKER), Card(Card.QUEEN, "Spades"), Card(Card.KING, "Clubs")})
table.foreach(discards, print)
expect_true(table.contains(discards, Card(Card.QUEEN, "Spades")))
expect_true(table.contains(discards, Card(Card.QUEEN, "Diamonds")))
expect_true(table.contains(discards, Card(Card.QUEEN, "Hearts")))


-- A more realistic hand
HAND = {
    Card(Card.ACE, "Hearts"), Card(7, "Hearts"), Card(6, "Hearts"),
    Card(Card.JOKER), Card(8, "Diamonds"), Card(4, "Diamonds"),
    Card(Card.JACK, "Clubs"), Card(Card.KING, "Spades"), Card(10, "Spades")
}
handDealt()
expect_eq(CONFIDENCE["Spades"], 7)

-- If my partner bids spades, expect me to bid up to 8 despite lower confidence
local bids = {{ player=PARTNER, bid=Bid(7,"Spades") },{ player=RIGHT, bid=Bid(7,"No Trumps") }}
local mybid = yourTurnToBid(bids)
table.insert(bids, {player=ME,bid=mybid})
expect_eq(bids[#bids].bid, Bid(8,"Spades"))
table.insert(bids, {player=RIGHT,bid=Bid(8,"Diamonds")})
bidWon(bids, RIGHT)

setTrumps("Diamonds")
setLeadSuit("Diamonds")
-- play big card as enemy winning
expect_eq(yourTurnToPlay({{ player=RIGHT, card=Card(10, "Diamonds") }}), Card("Joker"))
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Diamonds") },{ player=PARTNER, card=Card(Card.JACK, "Hearts") },{ player=RIGHT, card=Card(Card.JACK, "Diamonds")}}), Card("Joker"))
-- play little card as partner winning
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Diamonds") },{ player=PARTNER, card=Card(Card.JACK, "Diamonds") },{ player=RIGHT, card=Card(Card.JACK, "Hearts")}}), Card(4, "Diamonds"))
expect_eq(yourTurnToPlay({{ player=PARTNER, card=Card(Card.JACK, "Diamonds") },{ player=RIGHT, card=Card(Card.JACK, "Hearts")}}), Card(4, "Diamonds"))
table.delete(HAND, Card("Joker"))
-- want to win, but can't, so play little one
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Diamonds") },{ player=PARTNER, card=Card(Card.JACK, "Hearts") },{ player=RIGHT, card=Card(Card.JACK, "Diamonds")}}), Card(4, "Diamonds"))
-- offsuit round, we only have one of this suit
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Clubs") },{ player=PARTNER, card=Card(Card.ACE, "Clubs") },{ player=RIGHT, card=Card(6, "Clubs")}}), Card(Card.JACK, "Clubs"))
-- short ourselves in clubs
table.delete(HAND, Card(Card.JACK, "Clubs"))
-- partner winning, expect to play rubbish offsuit
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Clubs") },{ player=PARTNER, card=Card(Card.ACE, "Clubs") },{ player=RIGHT, card=Card(6, "Clubs")}}), Card(6, "Hearts"))
-- enemy winning, expect to try low trump
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Clubs") },{ player=PARTNER, card=Card(Card.KING, "Clubs") },{ player=RIGHT, card=Card(Card.ACE, "Clubs")}}), Card(4, "Diamonds"))
-- enemy winning by trump, try to overtrump
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Clubs") },{ player=PARTNER, card=Card(Card.KING, "Clubs") },{ player=RIGHT, card=Card(5, "Diamonds")}}), Card(8, "Diamonds"))
-- enemy winning by trump, impossible to overtrump, throw out rubbish
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Clubs") },{ player=PARTNER, card=Card(Card.KING, "Clubs") },{ player=RIGHT, card=Card("Joker")}}), Card(6, "Hearts"))

-- Your lead, play the winners
expect_eq(yourTurnToPlay({}), Card(Card.ACE, "Hearts"))
table.delete(HAND, Card(Card.ACE, "Hearts"))
-- no more winners, expect low card lead to partner
expect_eq(yourTurnToPlay({}), Card(6, "Hearts"))
-- notice your partner's short in something, don't play best card cos partner trumped
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Spades") },{ player=PARTNER, card=Card(4, "Diamonds") },{ player=RIGHT, card=Card(Card.QUEEN, "Spades")}}), Card(10, "Spades"))
-- notice your partner's short in something, play best card cos partner played offsuit
expect_eq(yourTurnToPlay({{ player=LEFT, card=Card(5, "Spades") },{ player=PARTNER, card=Card(4, "Hearts") },{ player=RIGHT, card=Card(Card.QUEEN, "Spades")}}), Card(Card.KING, "Spades"))
-- your turn to lead, lead to partner's offsuit
expect_eq(yourTurnToPlay({}):suit(), "Spades")

-- No Trumps play
HAND = {
    Card(Card.ACE, "Hearts"), Card(7, "Hearts"), Card(6, "Hearts"),
    Card(Card.JOKER), Card(8, "Diamonds"), Card(4, "Diamonds"),
    Card(Card.JACK, "Clubs"), Card(Card.KING, "Spades"), Card(10, "Spades")
}
setTrumps("")
setLeadSuit("")
bidWon({{ player=ME, bid=Bid(7,"No Trumps") }}, ME)
-- Play out winning cards first
expect_eq(yourTurnToPlay({}), Card(Card.ACE, "Hearts"))
table.delete(HAND, Card(Card.ACE, "Hearts"))
-- No winners, play joker as suit with nearest winner
expect_eq(yourTurnToPlay({}), Card(Card.JOKER, "Spades"))
-- If enemies are short, play that suit
SHORT_SUITED[LEFT]["Hearts"] = true
SHORT_SUITED[RIGHT]["Hearts"] = true
expect_eq(yourTurnToPlay({}):suit(), "Hearts")

-- Misere play
HAND = {
    Card(Card.ACE, "Hearts"), Card(7, "Hearts"), Card(6, "Hearts"),
    Card(Card.JOKER), Card(8, "Diamonds"), Card(4, "Diamonds"),
    Card(Card.JACK, "Clubs"), Card(Card.KING, "Spades"), Card(10, "Spades")
}
setTrumps("")
bidWon({{ player=ME, bid=Bid(Bid.CLOSED_MISERE) }}, ME)
-- lead with worst card
expect_eq(yourTurnToPlay({}), Card(4, "Diamonds"))
-- always play under
expect_eq(yourTurnToPlay({{player=RIGHT, card=Card(8, "Hearts")}}), Card(7, "Hearts"))
-- unless short suited
table.delete(HAND, Card(Card.JACK, "Clubs"))
expect_eq(yourTurnToPlay({{player=RIGHT, card=Card(10, "Clubs")}}), Card(Card.KING, "Spades"))





