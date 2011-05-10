--[[
	Jed.lua
	
	This is a very basic AI that provides an introduction to
	creating computer fivehundred players in Lua. It is not really
	capable of playing a real game. For that, see Patrick.lua

    Constants:
        "Spades"
        "Clubs"
        "Diamonds"
        "Hearts"
        Player.left
        Player.partner
        Player.right

    Methods
		Bid(str) -- str == "Pass", "Closed Misere", "Open Misere"
		Bid(num, suit)
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

function Player:handDealt()
--[[
    called when your hand was dealt.
    You can inspect self.hand to set up any initial conditions to use for later
--]]

    str_hand = "My hand is: "
    for _,c in ipairs(self.hand) do
        str_hand = str_hand..tostring(c)..", "
    end
    print(str_hand)
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
    return Bid("Pass")
end


function Player:bidWon(bids, winner)
--[[
    called when someone has won the bid.
    bids is a list of {player, bid} pairs that have been so far,
--]]
    if(winner == self or winner == Player.partner) then
        print("My team won the bid")
    else
        print("The opposing team won the bid")
    end
end

function Player:yourTurnToSelectKitty(kitty)
--[[
    called after you have won the bid.
    kitty is a table of 3 cards
    self.hand is a table of 10 cards
    return a table of 3 discards
--]]
	-- discard three random cards
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
        print("Hurrah!")
    end
end
