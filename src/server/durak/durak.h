#pragma once

#include <vector>
#include <memory>

#include "../actor/actor.h"
#include "../command.h"
#include "card.h"
#include "common/common.h"
#include "deck.h"

// Note: values do matter
enum class EDurakState : uint8_t {
    AttackerThinks = 0,
    DefenderThinks = 1
};

class TDurakGame {
public:
    const std::vector<std::unique_ptr<IActor>>& Actors;
    std::vector<std::vector<TCard>> Hands;

    std::vector<bool> IsPlaying;
    std::vector<unsigned> WinRating; // the first actor index is the winner, the last one is the loser
    bool Finished;

    TDeck Deck;
    ESuit Trump;

    EDurakState State;
    unsigned AttackingActor;
    std::vector<TCard> Table;
    std::optional<TCard> AttackingCard{};

public:
    TDurakGame(const std::vector<std::unique_ptr<IActor>>& actors)
        : Actors(actors)
        , Finished(false)
        , State(EDurakState::AttackerThinks)
        , AttackingActor(0) {
        auto trumpCard = Deck.GetBottom();
        Trump = trumpCard.suit;
        FreeFormBroadcast(-1, "Trump card (at the deck bottom): " + trumpCard.ToString());
        for (int i = 0; i < actors.size(); i++) {
            auto cards = Deck.Deal(6);
            Hands.push_back(cards);
            actors[i]->TookCards(cards);
            IsPlaying.push_back(true);
        }
        actors[GetActiveActor()]->MustAttack();
    }

    unsigned GetActiveActor() const;
    unsigned GetDefendingActor() const;

    void NotifyPlayerLeft(unsigned playerNum);

    TResult ExecutePlayerGameCommand(unsigned playerNum, TGameCommand cmd);

private:
    TResult ExecuteGameCommand(unsigned playerNum, TGameCommand cmd);

    void FillHand(unsigned playerNum);
    void CheckWin(unsigned playerNum);
    void FreeFormBroadcast(int excludeNum, const std::string& msg);
};