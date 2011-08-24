--[[

	Patrick.lua
	
	This is an example fivehundred AI that is capable of playing
	a proper game. For a simpler introduction to creating fivehundred
	AIs in Lua, see Jed.lua
]]

function oppositeSuit(suit)
    if suit == "Clubs" then return "Spades" end
    if suit == "Spades" then return "Clubs" end
    if suit == "Diamonds" then return "Hearts" end
    if suit == "Hearts" then return "Diamonds" end
end

function handDealt()
    local handStrength = {
        Spades = 0,
        Clubs = 0,
        Diamonds = 0,
        Hearts = 0,
        ["No Trumps"] = 0
    }
    function inc(suit, amount) handStrength[suit] = handStrength[suit] + amount end

    for _,c in ipairs(HAND) do
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
    
    PREFERRED_SUIT = 0
    PREFERRED_SUIT_STRENGTH = 0
    for suit, strength in pairs(handStrength) do
        if strength > PREFERRED_SUIT_STRENGTH then
            PREFERRED_SUIT = suit
            PREFERRED_SUIT_STRENGTH = strength
        end
    end
    VALIDSUITS = { Spades=true,Clubs=true,Diamonds=true,Hearts=true }
end

function yourTurnToBid(bids)
    print("Suit: "..PREFERRED_SUIT.." Strength: "..PREFERRED_SUIT_STRENGTH)
    local my_bid
    if PREFERRED_SUIT_STRENGTH >= 6 then
        if PREFERRED_SUIT_STRENGTH > 10 then
            PREFERRED_SUIT_STRENGTH = 10
        end
        my_bid = Bid(PREFERRED_SUIT_STRENGTH, PREFERRED_SUIT)
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


function bidWon(bids, winner)
	for _,p in ipairs(bids) do
                if p.player == winner and not(p.bid == Bid("Pass")) then
                        TRUMPS = p.bid:suit()
		end
	end
        print("Set trumps to "..TRUMPS)
end

function yourTurnToSelectKitty(kitty)
        table.sort(HAND)
    return {HAND[1], HAND[2], HAND[3]}
end

function lowestCard(suit)
	local lowest_card = nil
        for _, c in ipairs(HAND) do
                if (suit == nil or suit == c:suit()) and (lowest_card == nil or c < lowest_card) then
                        print("My lowest card is: "..tostring(c))
			lowest_card = c
		end
	end
	return lowest_card
end

function highestCard(suit)
	local highest_card = nil
        for _, c in ipairs(HAND) do
		if (suit == nil or suit == c:suit()) and (highest_card == nil or highest_card < c) then
                        print("My highest card is: "..tostring(c))
			highest_card = c
		end
	end
	return highest_card
end

function haveSuit(suit)
        print("Checking my hand for "..suit)
        for _, c in ipairs(HAND) do
		if c:suit() == suit then
                        print("Found "..tostring(c))
			return true
		end
	end
        print("I have no "..suit)
        table.foreach(HAND, print)
	return false
end

function yourTurnToPlay(trick)
    print("Cards in trick: "..#trick)
    if #trick == 0 then
        -- If no trumps and I have the joker, play it as the first suit I haven't thrown out on
        if TRUMPS == "" then for i, c in next, HAND do
            if c == Card(Card.JOKER) then
                for s,v in next,VALIDSUITS do if v then return Card(Card.JOKER, s) end end
            end
        end end
        -- I'm the first player, play anything
        return HAND[math.random(#HAND)]
    elseif #trick >= 2 then
                print("My partner has already played")
		-- partner has already played
		partners_card = trick[#trick-1].card
		partner_winning = true
		for _, p in ipairs(trick) do
			if partners_card < p.card then
				partner_winning = false
			end
		end

        lead_suit = trick[1].card:suit()
        if not haveSuit(lead_suit) then
                        VALIDSUITS[lead_suit] = false
			lead_suit = nil
		end
		print(lead_suit)
		if partner_winning then
                        print("Playing lowest card")
                        return lowestCard(lead_suit)
		else
                        print("Playing highest card")
                        return highestCard(lead_suit)
		end
	else
                print("My partner hasn't played yet")
		-- partner hasn't played
		lead_suit = trick[1].card:suit()
        if haveSuit(lead_suit) then
                        return highestCard(lead_suit)
		else
                VALIDSUITS[lead_suit] = false
                        if haveSuit(TRUMPS) then
                        print("lowest card trumps:",lowestCard(TRUMPS))
                                return lowestCard(TRUMPS)
			else
                        print("lowest card nil:",lowestCard(nil))
                                return lowestCard(nil)
			end
		end
    end
end


function trickWon(trick, winner)
end
