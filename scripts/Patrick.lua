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

function Player:handDealt()
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
    
    self.preferred_suit = 0
    self.preferred_suit_strength = 0
    for suit, strength in pairs(handStrength) do
        if strength > self.preferred_suit_strength then
            self.preferred_suit = suit
            self.preferred_suit_strength = strength
        end
    end
end

function Player:yourTurnToBid(bids)
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
	for _,p in ipairs(bids) do
		if p.player == winner then
			self.trumps = p.bid:suit()
		end
	end
end

function Player:yourTurnToSelectKitty(kitty)
	table.sort(self.hand)
    return {self.hand[1], self.hand[2], self.hand[3]}
end

function Player:lowestCard(suit)
	local lowest_card = nil
	for _, c in ipairs(self.hand) do
		if (suit == nil or suit == c:suit()) and (lowest_card == nil or c < lowest_card) then 
			lowest_card = c
		end
	end
	return lowest_card
end

function Player:highestCard(suit)
	local highest_card = nil
	for _, c in ipairs(self.hand) do
		if (suit == nil or suit == c:suit()) and (highest_card == nil or highest_card < c) then
			highest_card = c
		end
	end
	return highest_card
end

function Player:haveSuit(suit)
print("looking for "..suit)
	for _, c in ipairs(self.hand) do
		print(c)
		if c:suit() == suit then
			return true
		end
	end
	return false
end

function Player:yourTurnToPlay(trick)
print(#trick)
    if #trick == 0 then
        -- I'm the first player, play anything
        return self.hand[math.random(#self.hand)]
    elseif #trick >= 2 then
    print("partner played")
		-- partner has already played
		partners_card = trick[#trick-1].card
		partner_winning = true
		for _, p in ipairs(trick) do
			if partners_card < p.card then
				partner_winning = false
			end
		end

        lead_suit = trick[1].card:suit()
        if not self:haveSuit(lead_suit) then
			lead_suit = nil
		end
		print(lead_suit)
		if partner_winning then
			print("trying lowest card")
			return self:lowestCard(lead_suit)
		else
			print("tryping higehst")
			return self:highestCard(lead_suit)
		end
	else
	print("partner hasn't")
		-- partner hasn't played
		lead_suit = trick[1].card:suit()
        if self:haveSuit(lead_suit) then
			return self:highestCard(lead_suit)
		else
			if self:haveSuit(self.trumps) then
			print("lowest card trimps: "..tostring(self:lowestCard(self.trumps)))
				return self:lowestCard(self.trumps)
			else
			print("lowest card nil: "..tostring(self:lowestCard(nil)))
				return self:lowestCard(nil)
			end
		end
    end
end


function Player:trickWon(trick, winner)
end
