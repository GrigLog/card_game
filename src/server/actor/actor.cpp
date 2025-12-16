#include "actor.h"

#include "../durak/hand.h"
#include "../player_manager.h"

void TPlayer::FreeFormNotify(const std::string& msg) {
    TPlayerManager::SendToPlayer(Id, msg);
}

void TPlayer::CanDefend(const TCard& attackedBy) {
    FreeFormNotify("Attacked by: " + attackedBy.ToString());
}

void TPlayer::CanContinueAttack(const TCard& wasBeatenBy) {
    FreeFormNotify("Opponent defended with " + wasBeatenBy.ToString() + ". You may continue attacking.");
}

void TPlayer::TookCards(std::vector<TCard> cards) {
    FreeFormNotify("Took cards: " + Hand::ToString(cards));
}
