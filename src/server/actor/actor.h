#pragma once

#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "strategy.h"

class TCard;
namespace Hand {};

struct IActor {
    virtual bool IsPlayer() = 0;
    virtual bool IsOwner() {
        return false;
    };
    virtual std::string GetName() = 0;

    virtual void CanDefend(const TCard& attackedBy) {
    }
    virtual void MustAttack() {
    }
    virtual void CanContinueAttack(const TCard& wasBeatenBy) {
    }
    virtual void TookCards(std::vector<TCard> cards) {
    }

    virtual void FreeFormNotify(const std::string& msg) {
    }
};

struct TBot: IActor {
    std::unique_ptr<IBotStrategy> Strategy;
    unsigned RoomId;

    TBot(std::unique_ptr<IBotStrategy> strategy, unsigned roomId)
        : Strategy(std::move(strategy))
        , RoomId(roomId) {
    }

    bool IsPlayer() {
        return false;
    }
    std::string GetName() override {
        return "Bot" + std::to_string(RoomId);
    }
};

struct TPlayer: IActor {
    // using FCallback = std::function<void(unsigned, const std::string&)>;
    unsigned Id;
    bool Owner;
    // FCallback notifyCallback;

    TPlayer(unsigned id, bool bOwner /*, FCallback&& cb*/)
        : Id(id)
        , Owner(bOwner) /*, notifyCallback(cb)*/
    {
    }

    bool IsPlayer() override {
        return true;
    }
    bool IsOwner() override {
        return Owner;
    }
    std::string GetName() override {
        return "Player" + std::to_string(Id);
    }

    void FreeFormNotify(const std::string& msg) override;

    void CanDefend(const TCard& attackedBy) override;

    void MustAttack() override {
        FreeFormNotify("It's your turn to attack!");
    }
    void CanContinueAttack(const TCard& wasBeatenBy) override;
    void TookCards(std::vector<TCard> cards) override;
};