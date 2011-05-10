--[[
    Constants:
        "Spades"
        "Clubs"
        "Diamonds"
        "Hearts"
        Bid.PASS
        Bid.NORMAL
        Bid.CLOSED_MISERE
        Bid.OPEN_MISERE
        Player.left
        Player.partner
        Player.right

    Methods
        Bid:type()
        Bid:suit()
        Bid:tricks()
        Card:suit()
        Card:value()
        Card:isTrump()

    Metamethods:
        Suit.__eq(a,b)
        Bid.__eq(a,b)
        Bid.__lt(a,b)
        Card.__lt(a,b)

    Game sequence:
        handDealt()

        repeat {
            yourTurnToBid(current bids) -- return your new bid
        }
        bidWon(current bids)

        if you won the bid:
            yourTurnToSelectKitty(kitty) -- return your discarded cards

        repeat {
            yourTurnToPlay(current trick) -- return the card to play
            trickWon(trick, winner)
        }

        ...


		
	

]]

function oppositeSuit(suit)
    if suit == "Clubs" then return "Spades" end
    if suit == "Spades" then return "Clubs" end
    if suit == "Diamonds" then return "Hearts" end
    if suit == "Hearts" then return "Diamonds" end
end

function Player:handDealt()
--[[
    called when your hand was dealt.
    You can inspect self.hand to set up any initial conditions to use for later
--]]

    local handStrength = {
        Spades = 0,
        Clubs = 0,
        Diamonds = 0,
        Hearts = 0,
        ["No Trumps"] = 0
    }
    function inc(suit, amount) handStrength[suit] = handStrength[suit] + amount end

    for _,c in ipairs(self.hand) do
        if c:value() == Card.JOKER then
            inc("Spades", 2)
            inc("Clubs", 2)
            inc("Diamonds", 2)
            inc("Hearts", 2)
            inc("No Trumps", 3)
        elseif c:value() == Card.JACK then
            inc(c:suit(), 2)
            inc(oppositeSuit(c:suit()), 2)
        elseif c:value() == Card.ACE then
            inc("Spades", 1)
            inc("Clubs", 1)
            inc("Diamonds", 1)
            inc("Hearts", 1)
            inc("No Trumps", 1)
        elseif c:value() == Card.KING or c:value() == Card.QUEEN then
            inc("No Trumps", 1)
        else
            inc(c:suit(), 1)
        end
    end

    str_hand = "My hand is: "
    for _,c in ipairs(self.hand) do
        str_hand = str_hand..tostring(c)..", "
    end
    print(str_hand)
    --table.foreach(self.hand, print)
    --table.foreach(handStrength, print)
    self.preferred_suit = 0
    self.preferred_suit_strength = 0
    for suit, strength in pairs(handStrength) do
        print("In "..suit.." my strength is "..strength)
        if strength > self.preferred_suit_strength then
            self.preferred_suit = suit
            self.preferred_suit_strength = strength
        end
    end

end

function Player:yourTurnToBid(bids)
--[[
    called when it's your turn to bid.
    bids is a list of {player, bid} pairs that have been so far
    return your desired bid, e.g.
    return Bid.PASS
    return Bid.CLOSED_MISERE
    return 7, "Diamonds"
    return 10, "No Trumps"
--]]
    print("Suit: "..self.preferred_suit.." Strength: "..self.preferred_suit_strength)
    local my_bid
    if self.preferred_suit_strength >= 6 then
        if self.preferred_suit_strength > 10 then
            self.preferred_suit_strength = 10
        end
        my_bid = Bid(self.preferred_suit_strength, self.preferred_suit)
    else
        my_bid = Bid("Pass")
    end

    local max_bid = Bid("Pass")
    for _,pair in ipairs(bids) do
        if max_bid < pair.bid then
            max_bid = pair.bid
        end
    end

    if max_bid < my_bid then
        return my_bid
    else
        return Bid("Pass")
    end
end


function Player:bidWon(bids, winner)
--[[
    called when someone has won the bid.
    bids is a list of {player, bid} pairs that have been so far,
--]]
    if(winner == self or winner == Player.partner) then
        print("My team won the bid")
        self.tricksToWin = bids[#bids].bid:tricks()
    else
        print("The opposing team won the bid")
        self.tricksToWin = 11 - bids[#bids].bid:tricks()
    end
end

function Player:yourTurnToSelectKitty(kitty)
--[[
    called after you have won the bid.
    kitty is a table of 3 cards
    self.hand is a table of 10 cards
    return a table of 3 discards
--]]
    return { kitty[1], self.hand[2], self.hand[#self.hand] }
end

function Player:yourTurnToPlay(trick)
--[[
    called when it's your turn to play.
    trick is a list of { player, card } pairs
    return the card you wish to play
--]]
    if #trick == 0 then
        -- I'm the first player, play anything
        return self.hand[math.random(#self.hand)]
    else
        -- must follow suit
        lead_suit = trick[1].card:suit()
        for i,c in ipairs(self.hand) do
            if c:suit() == lead_suit then return c; end
        end
        -- short suited, return anything
        return self.hand[math.random(#self.hand)]
    end
end


function Player:trickWon(trick, winner)
--[[
    called when a trick is won
--]]
    if winner == self then
        print("hurrah!")
    end
end
