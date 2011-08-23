--[[
    Jed.lua

    This is a very basic AI that provides an introduction to
    creating computer fivehundred players in Lua. It is not really
    capable of playing a real game. For that, see Patrick.

    Suits:
        Suits are represented by string constants
        "Spades", "Clubs", "Diamonds", "Hearts"
        No trumps is an empty string.

    Card Values:
        Represented as integers. The values Card.JACK, Card.QUEEN, Card.KING, Card.ACE and Card.JOKER are also available

    Cards:
        c = Card(4, "Hearts")
        c = Card(Card.KING, "Spades")
        c = Card(Card.JOKER)
        c = Card(Card.JOKER, "Diamonds")
        When playing no trumps (or misere), you must set the suit of the joker as you play it.

        Cards have a very powerful less-than operator. It takes into account the current trump suit and the current lead card.
        For example, if trumps is spades and the lead card is diamonds
          Card(Card.KING, "Hearts") < Card(9, "Diamonds") < Card(5, "Spades")
        This allows you to do cool stuff like table.sort(HAND)
        For suits that are neither trumps nor the lead suit, cards are compared by value only.
        The joker is always appears the biggest card in this mechanism, event though it may not win if you play it as an offsuit in no trumps.

        The equality operator is also implemented. It returns true if the value and suit are equal, except in the case of the joker,
        where the suit is not considered. This means Card(Card.JOKER, "Spades") == Card(Card.JOKER, "Diamonds")

        Cards have the following methods:
        c:suit()
            This will take into account the current trump suit. For example, if trumps is diamonds:
            Card(Card.JACK, "Hearts"):suit() == "Diamonds"
            Card(Card.JOKER):suit() == "Diamonds"
        c:value()
            Returns a numeric value
        c:isTrump()
            Returns boolean, taking into account current trump suit.

        Cards can be printed as strings.

    Bids:
        b = Bid(Bid.PASS)
        b = Bid("Pass")
        b = Bid(8, "Spades")
        b = Bid(Bid.CLOSED_MISERE)
        b = Bid("Open Misere")

        The less-than operator is implemented for bids, allowing you to compare two bids easily.
        Bids have the following methods:
        b:type() is one of Bid.PASS, Bid.NORMAL, Bid.CLOSED_MISERE or Bid.OPEN_MISERE
        b:suit() see Suits, fatal error for type != Bid.NORMAL
        b:tricks() returns integer number of tricks, fatal error for type != Bid.NORMAL

    Globals:
        HAND : an array of the current cards in your hand
        ME, PARTNER, LEFT, RIGHT : keys to distinguish the players of each card

    Game sequence:
        handDealt()
        repeat through bidding process:
            yourTurnToBid(current bids)
        bidWon(current bids)
        if you won the bid:
            yourTurnToSelectKitty(kitty)
        repeat through playing process:
            yourTurnToPlay(current trick) -- return the card to play
            trickWon(trick, winner)

]]

handDealt = function()
--[[
    called when your hand was dealt.
    You can inspect HAND to set up any initial conditions to use for later
--]]

    str_hand = "My hand is: "
    for _,c in ipairs(HAND) do
        str_hand = str_hand..tostring(c)..", "
    end
    print(str_hand)
end

yourTurnToBid = function(bids)
--[[
    called when it's your turn to bid.
    bids is a list of {player, bid} pairs that have been so far
    return your desired bid
--]]
    return Bid("Pass")
end


bidWon = function(bids, winner)
--[[
    called when someone has won the bid.
    bids is a list of {player, bid} pairs
--]]
    if(winner == ME or winner == PARTNER) then
        print("My team won the bid")
    else
        print("The opposing team won the bid")
    end
end

yourTurnToSelectKitty = function(kitty)
--[[
    called after you have won the bid.
    kitty is a list of 3 cards
    HAND is a list of 10 cards
    you must return a table of 3 discards
--]]
    -- discard three random cards
    return { kitty[1], HAND[2], HAND[#HAND] }
end

yourTurnToPlay = function(trick)
--[[
    called when it's your turn to play.
    trick is a list of { player, card } pairs
    return the card you wish to play
--]]
    if #trick == 0 then
        -- I'm the first player, play anything
        local c = HAND[math.random(#HAND)]
        if c:value() == Card.JOKER then
            return Card(Card.JOKER, "Spades") -- always lead the joker as a spade
        else
            return c
        end
    else
        -- must follow suit
        lead_suit = trick[1].card:suit()
        for i,c in ipairs(HAND) do
            if c:suit() == lead_suit then return c; end
        end
        -- short suited, return anything
        return HAND[math.random(#HAND)]
    end
end


trickWon = function(trick, winner)
--[[
    called when a trick is won
--]]
    if winner == ME then
        print("Hurrah!")
    end
end
