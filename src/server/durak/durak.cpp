#include "durak.h"

#include <algorithm>

#include "common/common.h"
#include "hand.h"
#include "../player_manager.h"

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

void DurakGame::notifyPlayerLeft(unsigned playerNum) {
    // todo: replace with a bot
    isPlaying[playerNum] = false;
    int defendingActor = getDefendingActor();
    if (playerNum == attackingActor) {
        if (state == DurakState::AttackerThinks) {
            executeGameCommand(attackingActor, EndCommand());
        } else {
            executeGameCommand(defendingActor, TakeCommand()); // I'm sorry
        }
    } else if (playerNum == defendingActor) {
        if (state == DurakState::AttackerThinks) {
            executeGameCommand(attackingActor, EndCommand()); // I'm sorry
        } else {
            executeGameCommand(defendingActor, TakeCommand());
        }
    }
    hands.erase(hands.begin() + playerNum);
    isPlaying.erase(isPlaying.begin() + playerNum);
}

Result DurakGame::executePlayerGameCommand(unsigned playerNum, GameCommand cmd) {
    if (bFinished) {
        freeFormBroadcast(-1, "Game finished! Wait for the owner to close the room.");
    }
    auto res = executeGameCommand(playerNum, cmd);
    while (!bFinished && !actors[getActiveActor()]->isPlayer()) {
        if (auto bot = dynamic_cast<Bot*>(actors[getActiveActor()].get())) {
            GameCommand cmd = bot->strategy->generateCommand(*this);
            Result r = executeGameCommand(getActiveActor(), cmd);
            if (!r.first) {
                cmd = FallbackStrategy{}.generateCommand(*this);
                r = executeGameCommand(getActiveActor(), cmd);
                if (!r.first) {
                    std::cerr << "This bot is hardstuck" << std::endl;
                    bFinished = true;
                }
            }
        } else {
            std::cerr << "Something is terribly wrong with this bot" << std::endl;
            bFinished = true;
        }
    }
    if (actors.size() == 1 || attackingActor == getDefendingActor()) {
        freeFormBroadcast(-1, "Game finished! Wait for the owner to close the room.");
        bFinished = true;
    } else {
        actors[getActiveActor()]->freeFormNotify("It's your turn to " +
                                                 std::string(state == DurakState::AttackerThinks ? "attack" : "defend") + "\n" +
                                                 "Your hand: " + Hand::toString(hands[getActiveActor()]));
    }
    return res;
}

Result DurakGame::executeGameCommand(unsigned playerNum, GameCommand cmd) {
    unsigned active = getActiveActor();
    if (playerNum != active) {
        return {false, "It's not your turn yet. Wait for " + actors[active]->getName() + " to " +
                           (state == DurakState::AttackerThinks ? "attack" : "defend")};
    }
    std::vector<Card>& hand = hands[active];
    auto res = std::visit(
        VisitOverloadUtility{
            [&](SelectCommand c) -> Result {
                if (c.cardNum < 1 || c.cardNum > hand.size()) {
                    return {false, "Card num must be no less than 1 and no bigger than your hand size"};
                }
                int trueNum = c.cardNum - 1;
                Card selCard = hand[trueNum];
                if (state == DurakState::AttackerThinks) {
                    if (table.empty() || selCard.canAttack(table)) {
                        attackingCard = selCard;
                        table.push_back(selCard);
                        hand.erase(hand.begin() + trueNum);
                        freeFormBroadcast(active, "attacked with " + selCard.toString());
                        checkWin(active);
                        state = DurakState::DefenderThinks;
                        return {true, ""};
                    } else if (!table.empty()) {
                        return {false, "Can't add this card"};
                    }
                } else {
                    if (selCard.beats(attackingCard.value(), trump)) {
                        attackingCard = {};
                        table.push_back(selCard);
                        hand.erase(hand.begin() + trueNum);
                        freeFormBroadcast(active, "defended with " + selCard.toString());
                        checkWin(active);
                        state = DurakState::AttackerThinks;
                        return {true, ""};
                    } else {
                        return {false, "This card can't beat the attacking card"};
                    }
                }
                return {false, ""}; // Never actually returned but let's make the compiler happy
            },

            [&](TakeCommand c) -> Result {
                if (state == DurakState::AttackerThinks) {
                    return {false, "My brother in christ, you are attacking. You don't have to take cards."};
                } else {
                    std::string tableStr = Hand::toString(table);
                    attackingCard = {};
                    hand.append_range(table);
                    freeFormBroadcast(active, "took the cards: " + tableStr);
                    table.clear();
                    fillHand(attackingActor);
                    attackingActor = getDefendingActor(); // skip the turn
                    attackingActor = getDefendingActor();
                    state = DurakState::AttackerThinks;
                    return {true, ""};
                }
            },

            [&](EndCommand c) -> Result {
                if (state == DurakState::DefenderThinks) {
                    return {false, "My brother in christ, you are defending."};
                } else {
                    attackingCard = {};
                    table.clear();
                    freeFormBroadcast(active, "finished the attack");
                    fillHand(getDefendingActor());
                    fillHand(attackingActor);
                    attackingActor = getDefendingActor();
                    state = DurakState::AttackerThinks;
                    return {true, ""};
                }
            }},
        std::move(cmd));
    return res;
}

void DurakGame::fillHand(unsigned playerNum) {
    auto& hand = hands[playerNum];
    int missing = 6 - hand.size();
    if (missing <= 0) {
        return;
    }
    int willTake = std::min<int>(missing, static_cast<int>(deck.size()));
    if (willTake == 0) {
        return;
    }
    auto newCards = deck.deal(willTake);
    hand.append_range(newCards);
    actors[playerNum]->tookCards(newCards);
}

void DurakGame::checkWin(unsigned playerNum) {
    if (hands[playerNum].size() == 0 && deck.size() == 0) {
        isPlaying[playerNum] = false;
        winRating.push_back(playerNum);
        freeFormBroadcast(playerNum, "won! Took place: " + std::to_string(winRating.size()));
        bFinished = true;
    }
}

void DurakGame::freeFormBroadcast(int specialNum, const std::string& msg) {
    if (specialNum == -1) { // everyone gets the same message
        for (int i = 0; i < actors.size(); i++) {
            actors[i]->freeFormNotify(msg);
        }
    } else { // personalized message
        for (int i = 0; i < actors.size(); i++) {
            if (i == specialNum) {
                actors[i]->freeFormNotify("You " + msg);
            }
            if (i != specialNum) {
                actors[i]->freeFormNotify(actors[specialNum]->getName() + " " + msg);
            }
        }
    }
}
