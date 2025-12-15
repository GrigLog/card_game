#include "actor.h"

#include "../durak/hand.h"
#include "../player_manager.h"

void Player::freeFormNotify(const std::string& msg) {
    PlayerManager::sendToPlayer(id, msg);
}

void Player::canDefend(const Card& attackedBy) {
    freeFormNotify("Attacked by: " + attackedBy.toString());
}

void Player::canContinueAttack(const Card& wasBeatenBy) {
    freeFormNotify("Opponent defended with " + wasBeatenBy.toString() + ". You may continue attacking.");
}

void Player::tookCards(std::vector<Card> cards) {
    freeFormNotify("Took cards: " + Hand::toString(cards));
}
