#include "card_game/Action.h"

namespace card_game {

PlayCardAction::PlayCardAction(Card card) : card_(card) {}

std::string PlayCardAction::toString() const {
    return "PlayCard: " + card_.toString();
}

PassAction::PassAction() = default;

std::string PassAction::toString() const {
    return "Pass";
}

TakeCardsAction::TakeCardsAction() = default;

std::string TakeCardsAction::toString() const {
    return "TakeCards";
}

} // namespace card_game

