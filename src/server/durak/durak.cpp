#include "durak.h"
#include "common.h"
#include "hand.h"
#include <algorithm>

unsigned DurakGame::getActiveActor() const {
    return state == DurakState::AttackerThinks ? attackingActor : getDefendingActor();
}

unsigned DurakGame::getDefendingActor() const {
    int i = attackingActor;
    do {
        i = (i + 1) % actors.size();
    } while (i != attackingActor && !isPlaying[i]);
    return i;
}

Result DurakGame::executeAndBroadcastGameCommand(unsigned playerNum, GameCommand cmd) {
    auto res = executeGameCommand(playerNum, cmd);
    if (res.first) {
        auto name = actors[playerNum]->getName();
        freeFormBroadcast(playerNum, name + ": " + res.second);
    }
    if (attackingActor == getDefendingActor()) {
        freeFormBroadcast(-1, "Game finished!");
    } else {
        actors[getActiveActor()]->freeFormNotify("It's your turn to " + \
            std::string(state == DurakState::AttackerThinks ? "attack" : "defend") + "\n" + \
            "Your hand: " + Hand::toString(hands[getActiveActor()]));
    }
    return res;
}

Result DurakGame::executeGameCommand(unsigned playerNum, GameCommand cmd) {
    unsigned active = getActiveActor();
    if (playerNum != active)
        return {false, "It's not your turn yet. Wait for " + std::to_string(active) + " to " + \
        (state == DurakState::AttackerThinks ? "attack" : "defend")};
    std::vector<Card>& hand = hands[active];
    return std::visit(VisitOverloadUtility{
        [&](SelectCommand c) -> Result {
            if (c.cardNum < 1 || c.cardNum > hand.size())
                return {false, "Card num must be no less than 1 and no bigger than your hand size"};
            int trueNum = c.cardNum - 1;
            Card selCard = hand[trueNum];
            if (state == DurakState::AttackerThinks) {
                if (table.empty() || selCard.canAttack(table)) {
                    attackingCard = selCard;
                    table.push_back(selCard);
                    hand.erase(hand.begin() + trueNum);
                    checkWin(active);
                    state = DurakState::DefenderThinks;
                    return {true, "Attacked with " + selCard.toString()};
                } else if (!table.empty()) {
                    return {false, "Can't add this card"};
                }
            } else {
                if (selCard.beats(attackingCard.value(), trump)) {
                    attackingCard = {};
                    table.push_back(selCard);
                    hand.erase(hand.begin() + trueNum);
                    checkWin(active);
                    state = DurakState::AttackerThinks;
                    return {true, "Defended with " + selCard.toString()};
                } else {
                    return {false, "This card can't beat the attacking card"};
                }
            }
        },

        [&](TakeCommand c) -> Result {
            if (state == DurakState::AttackerThinks) {
                return {false, "My brother in christ, you are attacking. You don't have to take cards."};
            } else {
                std::string tableStr = Hand::toString(table);
                attackingCard = {};
                hand.append_range(table);
                table.clear();
                fillHand(attackingActor);
                attackingActor = getDefendingActor(); //skip the turn
                attackingActor = getDefendingActor();
                state = DurakState::AttackerThinks;
                return {true, "Took the cards: " + tableStr};
            }
        },

        [&](EndCommand c) -> Result {
            if (state == DurakState::DefenderThinks) {
                return {false, "My brother in christ, you are defending."};
            } else {
                attackingCard = {};
                table.clear();
                fillHand(getDefendingActor());
                fillHand(attackingActor);
                attackingActor = getDefendingActor();
                state = DurakState::AttackerThinks;
                return {true, "Attack has ended"};
            }
        }
    }, std::move(cmd));
}

void DurakGame::fillHand(unsigned playerNum) {
    auto& hand = hands[playerNum];
    int missing = 6 - hand.size();
    if (missing <= 0)
        return;
    int willTake = std::min<int>(missing, static_cast<int>(deck.size()));
    if (willTake == 0)
        return;
    auto newCards = deck.deal(willTake);
    hand.append_range(newCards);
    actors[playerNum]->tookCards(newCards);    
}

void DurakGame::checkWin(unsigned playerNum) {
    if (hands[playerNum].size() == 0 && deck.size() == 0) {
        isPlaying[playerNum] = false;
        winRating.push_back(playerNum);
        freeFormBroadcast(-1, 
            actors[playerNum]->getName() + " won! Took place: " + std::to_string(winRating.size()));
    }  
}

void DurakGame::freeFormBroadcast(int excludeNum, const std::string& msg) {
    for (int i = 0; i < actors.size(); i++) {
        if (i != excludeNum)
            actors[i]->freeFormNotify(msg);
    }
}
