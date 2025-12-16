#include "durak.h"

#include <algorithm>

#include "common/common.h"
#include "hand.h"
#include "../player_manager.h"

unsigned TDurakGame::GetActiveActor() const {
    return State == EDurakState::AttackerThinks ? AttackingActor : GetDefendingActor();
}

unsigned TDurakGame::GetDefendingActor() const {
    int i = AttackingActor;
    do {
        i = (i + 1) % Actors.size();
    } while (i != AttackingActor && !IsPlaying[i]);
    return i;
}

void TDurakGame::NotifyPlayerLeft(unsigned playerNum) {
    // todo: replace with a bot
    IsPlaying[playerNum] = false;
    int defendingActor = GetDefendingActor();
    if (playerNum == AttackingActor) {
        if (State == EDurakState::AttackerThinks) {
            ExecuteGameCommand(AttackingActor, TEndCommand());
        } else {
            ExecuteGameCommand(defendingActor, TTakeCommand()); // I'm sorry
        }
    } else if (playerNum == defendingActor) {
        if (State == EDurakState::AttackerThinks) {
            ExecuteGameCommand(AttackingActor, TEndCommand()); // I'm sorry
        } else {
            ExecuteGameCommand(defendingActor, TTakeCommand());
        }
    }
    Hands.erase(Hands.begin() + playerNum);
    IsPlaying.erase(IsPlaying.begin() + playerNum);
}

TResult TDurakGame::ExecutePlayerGameCommand(unsigned playerNum, TGameCommand cmd) {
    if (Finished) {
        FreeFormBroadcast(-1, "Game finished! Wait for the owner to close the room.");
    }
    auto res = ExecuteGameCommand(playerNum, cmd);
    while (!Finished && !Actors[GetActiveActor()]->IsPlayer()) {
        if (auto bot = dynamic_cast<TBot*>(Actors[GetActiveActor()].get())) {
            TGameCommand cmd = bot->Strategy->GenerateCommand(*this);
            TResult r = ExecuteGameCommand(GetActiveActor(), cmd);
            if (!r.first) {
                cmd = FallbackStrategy{}.GenerateCommand(*this);
                r = ExecuteGameCommand(GetActiveActor(), cmd);
                if (!r.first) {
                    std::cerr << "This bot is hardstuck" << std::endl;
                    Finished = true;
                }
            }
        } else {
            std::cerr << "Something is terribly wrong with this bot" << std::endl;
            Finished = true;
        }
    }
    if (Actors.size() == 1 || AttackingActor == GetDefendingActor()) {
        FreeFormBroadcast(-1, "Game finished! Wait for the owner to close the room.");
        Finished = true;
    } else {
        Actors[GetActiveActor()]->FreeFormNotify("It's your turn to " +
                                                 std::string(State == EDurakState::AttackerThinks ? "attack" : "defend") + "\n" +
                                                 "Your hand: " + Hand::ToString(Hands[GetActiveActor()]));
    }
    return res;
}

TResult TDurakGame::ExecuteGameCommand(unsigned playerNum, TGameCommand cmd) {
    unsigned active = GetActiveActor();
    if (playerNum != active) {
        return {false, "It's not your turn yet. Wait for " + Actors[active]->GetName() + " to " +
                           (State == EDurakState::AttackerThinks ? "attack" : "defend")};
    }
    std::vector<TCard>& hand = Hands[active];
    auto res = std::visit(
        TVisitOverloadUtility{
            [&](TSelectCommand c) -> TResult {
                if (c.CardNum < 1 || c.CardNum > hand.size()) {
                    return {false, "Card num must be no less than 1 and no bigger than your hand size"};
                }
                int trueNum = c.CardNum - 1;
                TCard selCard = hand[trueNum];
                if (State == EDurakState::AttackerThinks) {
                    if (Table.empty() || selCard.CanAttack(Table)) {
                        AttackingCard = selCard;
                        Table.push_back(selCard);
                        hand.erase(hand.begin() + trueNum);
                        FreeFormBroadcast(active, "attacked with " + selCard.ToString());
                        CheckWin(active);
                        State = EDurakState::DefenderThinks;
                        return {true, ""};
                    } else if (!Table.empty()) {
                        return {false, "Can't add this card"};
                    }
                } else {
                    if (selCard.Beats(AttackingCard.value(), Trump)) {
                        AttackingCard = {};
                        Table.push_back(selCard);
                        hand.erase(hand.begin() + trueNum);
                        FreeFormBroadcast(active, "defended with " + selCard.ToString());
                        CheckWin(active);
                        State = EDurakState::AttackerThinks;
                        return {true, ""};
                    } else {
                        return {false, "This card can't beat the attacking card"};
                    }
                }
                return {false, ""}; // Never actually returned but let's make the compiler happy
            },

            [&](TTakeCommand c) -> TResult {
                if (State == EDurakState::AttackerThinks) {
                    return {false, "My brother in christ, you are attacking. You don't have to take cards."};
                } else {
                    std::string tableStr = Hand::ToString(Table);
                    AttackingCard = {};
                    hand.append_range(Table);
                    FreeFormBroadcast(active, "took the cards: " + tableStr);
                    Table.clear();
                    FillHand(AttackingActor);
                    AttackingActor = GetDefendingActor(); // skip the turn
                    AttackingActor = GetDefendingActor();
                    State = EDurakState::AttackerThinks;
                    return {true, ""};
                }
            },

            [&](TEndCommand c) -> TResult {
                if (State == EDurakState::DefenderThinks) {
                    return {false, "My brother in christ, you are defending."};
                } else {
                    AttackingCard = {};
                    Table.clear();
                    FreeFormBroadcast(active, "finished the attack");
                    FillHand(GetDefendingActor());
                    FillHand(AttackingActor);
                    AttackingActor = GetDefendingActor();
                    State = EDurakState::AttackerThinks;
                    return {true, ""};
                }
            }},
        std::move(cmd));
    return res;
}

void TDurakGame::FillHand(unsigned playerNum) {
    auto& hand = Hands[playerNum];
    int missing = 6 - hand.size();
    if (missing <= 0) {
        return;
    }
    int willTake = std::min<int>(missing, static_cast<int>(Deck.Size()));
    if (willTake == 0) {
        return;
    }
    auto newCards = Deck.Deal(willTake);
    hand.append_range(newCards);
    Actors[playerNum]->TookCards(newCards);
}

void TDurakGame::CheckWin(unsigned playerNum) {
    if (Hands[playerNum].size() == 0 && Deck.Size() == 0) {
        IsPlaying[playerNum] = false;
        WinRating.push_back(playerNum);
        FreeFormBroadcast(playerNum, "won! Took place: " + std::to_string(WinRating.size()));
        Finished = true;
    }
}

void TDurakGame::FreeFormBroadcast(int specialNum, const std::string& msg) {
    if (specialNum == -1) { // everyone gets the same message
        for (int i = 0; i < Actors.size(); i++) {
            Actors[i]->FreeFormNotify(msg);
        }
    } else { // personalized message
        for (int i = 0; i < Actors.size(); i++) {
            if (i == specialNum) {
                Actors[i]->FreeFormNotify("You " + msg);
            }
            if (i != specialNum) {
                Actors[i]->FreeFormNotify(Actors[specialNum]->GetName() + " " + msg);
            }
        }
    }
}
