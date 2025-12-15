#pragma once

#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "strategy.h"

struct Card;
namespace Hand {};

struct IActor {
    virtual bool isPlayer() = 0;
    virtual bool isOwner() {
        return false;
    };
    virtual std::string getName() = 0;

    virtual void canDefend(const Card& attackedBy) {
    }
    virtual void mustAttack() {
    }
    virtual void canContinueAttack(const Card& wasBeatenBy) {
    }
    virtual void tookCards(std::vector<Card> cards) {
    }

    virtual void freeFormNotify(const std::string& msg) {
    }
};

struct Bot: IActor {
    std::unique_ptr<IBotStrategy> strategy;
    unsigned roomId;

    Bot(std::unique_ptr<IBotStrategy> strategy, unsigned roomId)
        : strategy(std::move(strategy))
        , roomId(roomId) {
    }
    bool isPlayer() {
        return false;
    }
    std::string getName() override {
        return "Bot" + std::to_string(roomId);
    }
};

struct Player: IActor {
    // using FCallback = std::function<void(unsigned, const std::string&)>;
    unsigned id;
    bool bOwner;
    // FCallback notifyCallback;

    Player(unsigned id, bool bOwner /*, FCallback&& cb*/)
        : id(id)
        , bOwner(bOwner) /*, notifyCallback(cb)*/ {
    }
    bool isPlayer() override {
        return true;
    }
    bool isOwner() override {
        return bOwner;
    }
    std::string getName() override {
        return "Player" + std::to_string(id);
    }

    void freeFormNotify(const std::string& msg) override;

    void canDefend(const Card& attackedBy) override;

    void mustAttack() override {
        freeFormNotify("It's your turn to attack!");
    }
    void canContinueAttack(const Card& wasBeatenBy) override;
    void tookCards(std::vector<Card> cards) override;
};