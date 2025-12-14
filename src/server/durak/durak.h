#pragma once
#include <vector>
#include <memory>
#include "../actor/actor.h"
#include "../command.h"
#include "card.h"
#include "deck.h"
#include "common.h"

//Note: values do matter
enum class DurakState : uint8_t {
    AttackerThinks = 0,
    DefenderThinks = 1
};

struct DurakGame {
    const std::vector<std::unique_ptr<IActor>>& actors;
    std::vector<std::vector<Card>> hands;

    std::vector<bool> isPlaying;
    std::vector<unsigned> winRating; //the first actor index is the winner, the last one is the loser
    
    Deck deck;
    Suit trump;

    DurakState state = DurakState::AttackerThinks;
    unsigned attackingActor = 0;
    std::vector<Card> table;
    std::optional<Card> attackingCard{};

    unsigned getActiveActor() const;
    unsigned getDefendingActor() const;

    DurakGame(const std::vector<std::unique_ptr<IActor>>& actors) : actors(actors) {
        auto trumpCard = deck.getBottom();
        trump = trumpCard.suit;
        for (int i = 0; i < actors.size(); i++) {
            auto cards = deck.deal(6);
            hands.push_back(cards);
            actors[i]->tookCards(cards);
            actors[i]->freeFormNotify("Trump card (at the deck bottom): " + trumpCard.toString());
            isPlaying.push_back(true);
        }
        actors[getActiveActor()]->mustAttack();
    }

    void notifyPlayerLeft(unsigned playerNum);

    Result executeAndBroadcastGameCommand(unsigned playerNum, GameCommand cmd);

private:
    Result executeGameCommand(unsigned playerNum, GameCommand cmd);

    void fillHand(unsigned playerNum);
    void checkWin(unsigned playerNum);
    void freeFormBroadcast(int excludeNum, const std::string& msg);
};