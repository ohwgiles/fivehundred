
sameColour = {
    Spades    = 'Clubs',
    Clubs     = 'Spades',
    Diamonds  = 'Hearts',
    Hearts    = 'Diamonds'
}

--CONFIDENCE = {}
--ENEMY_STRENGTH = nil
--PARTNER_STRENGTH = nil
--BID_WINNER = nil
--WINNING_BID = nil
--TRICKS_REQUIRED = 0
--TRUMP_SUIT = ""
--CARDS_PLAYED = {}
--WINNING_CARDS = {}
--SHORT_SUITED = {}
SUITS = {"Spades", "Clubs", "Diamonds", "Hearts"}
--NONTRUMPSUITS = {}

function table.copy(to, from)
    for k,v in next, from do
        to[k] = v
    end
end

function table.contains(tbl, obj, cmp_)
    cmp = cmp_ or function(o1,o2)
        return o1 == o2
    end
    for _, e in next, tbl do
    --print("comparing",e,"with",obj)
        if cmp(e, obj) then
            return true
        end
    end
    return false
end

function table.find(tbl, obj, cmp_)
    cmp = cmp_ or function(o1,o2)
        return o1 == o2
    end
    for k, v in next, tbl do
        if cmp(v, obj) then
            return k
        end
    end
    return nil
end


function table.all(tbl, matchFn)
    local result = {}
    for _, e in next, tbl do
        if matchFn(e) then
            table.insert(result, e)
        end
    end
    return result
end

function table.delete(tbl, obj)
    for i = #tbl, 1, -1 do
        if tbl[i] == obj then
            return table.remove(tbl, i)
        end
    end
end

function cards(suit, cardGroup)
    tbl = cardGroup or HAND
    return table.all(tbl, function(c) return c:suit() == suit end)
end

function haveAny(suit, cardGroup)
    tbl = cardGroup or HAND
    return table.contains(tbl, suit, function(c) return c:suit() == suit end)
end

function handDealt()
    CONFIDENCE = {}
    local strength = {}
    local numRuns = 0
    local totalRunLength = 0
    local joker = table.contains(HAND, Card(Card.JOKER)) and 1 or 0
    print("Joker:",joker)
    CONFIDENCE["Misere"] = 8 - joker
    for _, suit in next, SUITS do
        strength[suit] = {
            run = 0,
            sum = 0,
            count = 0,
            jack = false
        }
        local cardsInSuit = cards(suit)
        strength[suit].count = #cardsInSuit
        table.sort(cardsInSuit)
        for _, c in next, cardsInSuit do
            strength[suit].sum = strength[suit].sum + c:value()
        end
        local netHoles = 0
        local lowestCardVal = (suit == "Diamonds" or suit == "Hearts") and Card.FOUR or Card.FIVE
        if #cardsInSuit > 0 then
            for i = lowestCardVal, cardsInSuit[#cardsInSuit]:value() do
                if not table.contains(cardsInSuit, Card(i, suit)) then
                    netHoles = netHoles + 1
                end
            end
            if netHoles > 0 then
                print("net holes is", netHoles, ", factor in", suit, "is", (#cardsInSuit - netHoles) / #cardsInSuit)
                CONFIDENCE["Misere"] = CONFIDENCE["Misere"] + (#cardsInSuit - netHoles) / #cardsInSuit
            end
        else
            CONFIDENCE["Misere"] = CONFIDENCE["Misere"] + 1
        end
        for v = Card.ACE, Card.FOUR, -1 do
            if table.contains(HAND, Card(v, suit)) then
                strength[suit].run = strength[suit].run + 1
            else
                break
            end
        end
        if strength[suit].run > 0 then
            numRuns = numRuns + 1
            totalRunLength = totalRunLength + strength[suit].run
        end
        strength[suit].jack = table.contains(HAND, Card(Card.JACK, suit)) and 0.5 or 0
    end
    for _, suit in next, SUITS do
        CONFIDENCE[suit] = math.floor(0.5 +
            2*joker + strength[suit].count + strength[suit].sum/20 +
            strength[sameColour[suit] ].jack + (strength[suit].run > 0 and numRuns-1 or numRuns)
        )
        print("CONFIDENCE in "..suit.." is", CONFIDENCE[suit])
        if CONFIDENCE[suit] > 10 then
            CONFIDENCE[suit] = 10
        end
    end
    CONFIDENCE["No Trumps"] = 2*joker + totalRunLength + 3
    if joker ~= 0 then
        if CONFIDENCE["No Trumps"] > 10 then
            CONFIDENCE["No Trumps"] = 10
        end
    else
        if CONFIDENCE["No Trumps"] > 7 then
            CONFIDENCE["No Trumps"] = 7
        end
    end
    print("CONFIDENCE in No Trumps is", CONFIDENCE["No Trumps"])
    print("CONFIDENCE in Misere is", CONFIDENCE["Misere"])

    str_hand = "My hand is: "
    for _,c in next, HAND do str_hand = str_hand..tostring(c)..", " end
    print(str_hand)
end

function calculateNextBid(bidToBeat)
    local suit = nil
    local tricks = 0
    local possibles = {}
    print("Aiming to beat", bidToBeat)
    for _, suit in next, {"Spades", "Clubs", "Diamonds", "Hearts", "No Trumps"} do
        local v = CONFIDENCE[suit]
        if suit == PARTNER_STRENGTH then
            v = v < 10 and v+1 or v
        elseif suit == ENEMY_STRENGTH then
            v = v-1
        end
        for i = 6,v do
            local b = Bid(i, suit)
            if bidToBeat < b then
                return b
            end
        end
    end
    -- only call misere as a fallback
    if CONFIDENCE["Misere"] > 5 and bidToBeat < Bid(Bid.CLOSED_MISERE) then
        return Bid(Bid.CLOSED_MISERE)
    end
    if CONFIDENCE["Misere"] > 8 and bidToBeat < Bid(Bid.OPEN_MISERE) then
        return Bid(Bid.OPEN_MISERE)
    end
    return Bid(Bid.PASS)
end

function yourTurnToBid(bids)
    if #bids == 0 then
        return calculateNextBid(Bid(Bid.PASS))
    end

    winner = bids[1]
    print(winner.bid)
    for _,p in next, bids do
        print(p.bid)
        if p.bid:type() == Bid.NORMAL then
            if p.player == PARTNER then
                PARTNER_STRENGTH = p.bid:suit()
            else
                ENEMY_STRENGTH = p.bid:suit()
            end
        end

        if winner.bid < p.bid then
            winner = p
        end
    end
    print("Winning bid is",winner.bid,"by",winner.player)
    if winner.player == ME then
        print("I'm winning so pass")
        return Bid(Bid.PASS)
    end
    return calculateNextBid(winner.bid)
end


function bidWon(bids, winner)
    BID_WINNER = winner
    for i = #bids,0,-1 do
        if bids[i].bid:type() ~= Bid.PASS then
            WINNING_BID = bids[i].bid
            break
        end
    end
    if WINNING_BID:type() == Bid.NORMAL then
        TRUMP_SUIT = WINNING_BID:suit()
        NONTRUMPSUITS = {}
        table.copy(NONTRUMPSUITS, SUITS)
        table.delete(NONTRUMPSUITS, TRUMP_SUIT)
    else
        TRUMP_SUIT = nil
    end
    SHORT_SUITED = {}
    SHORT_SUITED[PARTNER] = {}
    SHORT_SUITED[LEFT] = {}
    SHORT_SUITED[RIGHT] = {}
    SHORT_SUITED[ME] = {}
    CARDS_PLAYED = {}
end

function setCardPlayed(card)
    table.insert(CARDS_PLAYED, card)
    return card
end

function isCardPlayed(card)
    return table.find(CARDS_PLAYED, card)
end

selectKittyNormal = function(kitty)
    crap = {}
    for _, c in next, kitty do
        table.insert(HAND, c)
    end
    for _, suit in next, NONTRUMPSUITS do
        local inRun = true
        for v = Card.ACE, Card.FOUR, -1 do
            local c = Card(v,suit)
            if not inRun and table.contains(HAND, c) then
                table.insert(crap, c)
            elseif inRun and not table.contains(HAND, c) then
                inRun = false
            end
        end
    end
    table.sort(crap)
    local desireToRid = {}
    local suitValue = {}
    for _, suit in next, NONTRUMPSUITS do
        desireToRid[suit] = 0
        suitValue[suit] = 0
        local cds = cards(suit, crap)
        print("I have", #cds, "crap", suit)
        if #cds > 0 then
            if #cds <= 3 then
                desireToRid[suit] = desireToRid[suit] + 4 - #cds
            end
            if suit == ENEMY_STRENGTH then
                desireToRid[suit] = desireToRid[suit] + 2
            end
            for _,c in next, cds do
                suitValue[suit] = suitValue[suit] + c:value()
            end
        end
        print("Desire to rid of "..suit.." is", desireToRid[suit], ", strength is", suitValue[suit])
    end
    local discards = {}
    table.sort(NONTRUMPSUITS, function(suit1, suit2) return desireToRid[suit1] > desireToRid[suit2] or (desireToRid[suit1] == desireToRid[suit2] and suitValue[suit1] < suitValue[suit2]); end)
    table.sort(HAND)
    table.foreach(crap,print)
    for _, suit in next, NONTRUMPSUITS do
        if haveAny(suit, crap) then
            for k,c in next, crap do
                print(suit, ":", c)
                if c:suit() == suit and (#discards < 3) then
                print("discarding",c)
                    table.insert(discards, c)
                    setCardPlayed(discards[#discards])
                end
            end
        end
    end
    for i = 1, #discards do
        table.delete(HAND, discards[i])
    end
    for i = #discards, 2 do
        table.insert(discards, table.remove(HAND, 1))
    end
    print("leaving selectKittyNormal")
    return discards
end

numHoles = function(card)
    local holes = 0
    suit = card:suit()
    local lowestValue = (suit == "Diamonds" or suit == "Hearts") and Card.FOUR or Card.FIVE
    for i = card:value()-1, lowestValue, -1 do
        if not table.contains(HAND, Card(i, suit)) then
            holes = holes + 1
        end
    end
    return holes
end

selectKittyMisere = function(kitty)
    for _, c in next, kitty do
        table.insert(HAND, c)
    end
    local discards = {}
    local numToDiscard = 3
    -- Always discard the joker
    if table.contains(HAND, Card(Card.JOKER)) then
        table.insert(discards, table.delete(HAND, Card(Card.JOKER)))
        numToDiscard = 2
    end
    table.sort(HAND, function(c1, c2)
        local c1holes = numHoles(c1)
        local c2holes = numHoles(c2)
        print(c1,c1holes,c2,c2holes)
        if c1holes == c2holes then return c1:value() > c2:value()
        else return c1holes > c2holes end
    end)
    for i = 1, numToDiscard do
        table.insert(discards, table.remove(HAND, 1))
    end
    return discards
end

function yourTurnToSelectKitty(kitty)
    local d = {}
    if WINNING_BID:type() == Bid.NORMAL then
        d = selectKittyNormal(kitty)
    else
        d = selectKittyMisere(kitty)
    end
    table.foreach(d, print)
    return d
end

function findWinners()
    local winners = {}
    for _, suit in next, SUITS do
        local best = Card.ACE
        if suit == TRUMP_SUIT then
            best = Card.JOKER
        end
        for i = best, Card.FOUR, -1 do
            if i == Card.JACK and (suit == sameColour[TRUMP_SUIT] or suit == TRUMP_SUIT) then
                -- skip, already covered by decrementing from "best"
            else
                local c = Card(i, suit)
                if table.contains(HAND, c) then
                    table.insert(winners, c)
                elseif not isCardPlayed(c) then
                    break
                end
            end
        end
    end
    return winners
end

function splitHandIntoSuits()
    return {
        Spades = cards("Spades"),
        Clubs = cards("Clubs"),
        Diamonds = cards("Diamonds"),
        Hearts = cards("Hearts")
    }
end

function rubbish(suit)
    print("Unable to find a suitable card, chuck out some rubbish")
    if suit == nil then
        table.sort(HAND)
        table.foreach(HAND, print)
        return HAND[1]
    else
        local possibles = cards(suit)
        if #possibles == 0 then
            return rubbish()
        end
        table.sort(possibles)
        return possibles[1]
    end
end


function leadCardNoTrumps()
    local winners = findWinners()
    table.foreach(winners, print)
    if #winners > 0 then
        table.sort(winners)
        return winners[#winners]
    end

    print("Play something both enemies are short in")
    for _, suit in next, SUITS do
        local cardsOfSuit = cards(suit)
        if #cardsOfSuit > 0 and SHORT_SUITED[LEFT][suit] and SHORT_SUITED[RIGHT][suit] then
            return cardsOfSuit[#cardsOfSuit]
        end
    end

    print("We have the joker?")
    if table.contains(HAND, Card(Card.JOKER)) then
        -- find suit that has the biggest card
        if #HAND == 1 then for _,s in next,SUITS do
            if not SHORT_SUITED[ME][s] then return Card(Card.JOKER, s) end
        end end
        table.sort(HAND)
        local suitWithBiggestCard = HAND[#HAND - 1]:suit()
        -- play joker as that suit
        print("Want to play joker as a", suitWithBiggestCard)
        return Card(Card.JOKER, suitWithBiggestCard)
    end

    print("Play something partner isn't short in")
    for _, suit in next, SUITS do
        local cardsOfSuit = cards(suit)
        if #cardsOfSuit > 0 and not SHORT_SUITED[PARTNER][suit] then
            return cardsOfSuit[1]
        end
    end
end

function leadCardTrumps()
    local winners = findWinners()
    print("my lead, have any winners?")
    if #winners > 0 then
        for _, suit in next, NONTRUMPSUITS do
            local winnersOfSuit = cards(suit, winners)
            if #winnersOfSuit > 0 and (not SHORT_SUITED[LEFT][suit] or SHORT_SUITED[LEFT][TRUMP_SUIT]) and (not SHORT_SUITED[RIGHT][suit] or SHORT_SUITED[RIGHT][TRUMP_SUIT]) then
                print("have winners")
                table.sort(winnersOfSuit)
                table.foreach(winnersOfSuit, print)
                return winnersOfSuit[#winnersOfSuit]
            end
        end
        print("we only have winners in trumps")
        print("bid winner: ", BID_WINNER, "me: ", ME)
        if BID_WINNER == ME or BID_WINNER == PARTNER then
            table.sort(winners)
            return winners[#winners]
        end
    end

    -- we must have decided we have nothing good enough
    local eachSuit = splitHandIntoSuits()
    print("no winners, try to get partner to win")
    print("bid winner: ", BID_WINNER, "me: ", ME)
    if (BID_WINNER == ME or BID_WINNER == PARTNER) and #eachSuit[TRUMP_SUIT] > 0 and not SHORT_SUITED[PARTNER][TRUMP_SUIT] then
        print("try lead low trump")
        trumpCards = cards(TRUMP_SUIT)
        table.sort(trumpCards)

        return trumpCards[1]
    else
        print("play a suit you know your partner's short in")
        for _, suit in next, NONTRUMPSUITS do
            if #eachSuit[suit] > 0 and SHORT_SUITED[PARTNER][suit] and not SHORT_SUITED[RIGHT][suit] then
                table.sort(eachSuit[suit])

                return eachSuit[suit][1]
            end
        end
        print("throw out a single card of a non-trump suit")
        for _, suit in next, NONTRUMPSUITS do
            if #eachSuit[suit] == 1 then

                return eachSuit[suit][1]
            end
        end
        if (BID_WINNER == ME or BID_WINNER == PARTNER) and #eachSuit[TRUMP_SUIT] > 0 then
            print("fish for trumps")
            table.sort(eachSuit[TRUMP_SUIT])

            return eachSuit[TRUMP_SUIT][1]
        end
    end
end

function updatePlayedCards(trick)
    local lead_suit = trick[1].card:suit()
    print(lead_suit)
    for _,t in next,trick do
        setCardPlayed(t.card)
        if t.card:suit() ~= lead_suit then
            print("Player",t.player,"is short in",lead_suit)
            SHORT_SUITED[t.player][lead_suit] = true
        end
    end
end

function playingTrumps()
    return WINNING_BID:type() == Bid.NORMAL and WINNING_BID:suit() ~= ""
end

function playCardMisere(trick)
    local lead_player = trick[1].player
    local lead_card = trick[1].card
    local lead_suit = lead_card:suit()
    local eachSuit = splitHandIntoSuits()

    if #eachSuit[lead_suit] > 0 then
        print("have to follow suit")
        --play the biggest one we have that is still below the winning one
        table.sort(eachSuit[lead_suit])
        for i = #eachSuit[lead_suit], 1, -1 do
            print("Looking at", eachSuit[lead_suit][i], "against", lead_card)
            if eachSuit[lead_suit][i] < lead_card then return eachSuit[lead_suit][i] end
        end
        -- hosed, play biggest, or joker if we have it
        if table.contains(HAND, Card(Card.JOKER)) then
            return Card(Card.JOKER, lead_suit)
        else
            return eachSuit[lead_suit][ #eachSuit[lead_suit] ]
        end
    end

    print("We have none of that suit, let's throw out biggest from suit with least cards")
    maxCards = 11
    maxSuit = nil
    for _, suit in next, SUITS do
        if #eachSuit[suit] > 0 and #eachSuit[suit] < maxCards then
            maxCards = #eachSuit[suit]
            maxSuit = suit
        end
    end
    if maxSuit == nil then return Card(Card.JOKER) end
    print("max suit: ", maxSuit, "with cards:", maxCards)
    table.sort(eachSuit[maxSuit])
    return eachSuit[maxSuit][ #eachSuit[maxSuit] ]
end

function playCardNormal(trick)
    local lead_player = trick[1].player
    local lead_suit = trick[1].card:suit()
    -- sort the cards so we know who's winning
    table.sort(trick, function(p1,p2) return p1.card < p2.card end)
    if trick[#trick].player == PARTNER then
        print("Partner winning, play low card")
        return nil
    end

    print("Enemy winning, attempt to beat")
    local eachSuit = splitHandIntoSuits()
    -- if no trumps, consider the joker to be one of this suit rather than of none, but only if we haven't played off suit already
    if not playingTrumps() and table.contains(HAND, Card(Card.JOKER)) and not SHORT_SUITED[ME][lead_suit] then
        print("Setting the joker as a", lead_suit)
        table.insert(eachSuit[lead_suit], Card(Card.JOKER, lead_suit))
    end

    if #eachSuit[lead_suit] > 0 then
        print("have to follow suit")
        table.sort(eachSuit[lead_suit])
        if eachSuit[lead_suit][ #eachSuit[lead_suit] ] > trick[#trick].card then
            local toPlay = eachSuit[lead_suit][ #eachSuit[lead_suit] ]
            return toPlay
        else
            print("I don't have a card to beat that")
            return eachSuit[lead_suit][1]
        end
    end

    if playingTrumps() and lead_suit ~= TRUMP_SUIT and #eachSuit[TRUMP_SUIT] > 0 then
        print("opportunity to play trump")
        if #trick < 3 and SHORT_SUITED[LEFT][TRUMP_SUIT] then
            print("next guy is short in these! play a biggie")
            table.sort(eachSuit[TRUMP_SUIT])
            return eachSuit[TRUMP_SUIT][ #eachSuit[TRUMP_SUIT] ]
        else
            print("using first low trump that wins")
            table.sort(eachSuit[TRUMP_SUIT])
            for _, c in next, eachSuit[TRUMP_SUIT] do
                if trick[#trick].card < c then return c end
            end
        end
    end

end

function yourTurnToPlay(trick)
    local myCard = nil
    local lead_suit = nil
    if #trick == 0 then
        if playingTrumps() then
            myCard = leadCardTrumps(trick)
        elseif WINNING_BID:type() == Bid.NORMAL then
            myCard = leadCardNoTrumps(trick)
        end
        -- my lead of misere will fall out to playing rubbish()
    else
        lead_suit = trick[1].card:suit()
        updatePlayedCards(trick)
        if WINNING_BID:type() == Bid.CLOSED_MISERE or WINNING_BID:type() == Bid.OPEN_MISERE then
            myCard = playCardMisere(trick)
        else
            myCard = playCardNormal(trick)
        end
    end

    if myCard == nil then
        myCard = rubbish(lead_suit)
    end
    return setCardPlayed(myCard)
end


function trickWon(trick, winner)
    updatePlayedCards(trick)
end
