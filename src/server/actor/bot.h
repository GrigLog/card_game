#pragma once
#include "actor.h"
#include "strategy.h"


struct Bot : IActor {
    std::unique_ptr<IBotStrategy> strategy;
    unsigned roomId;

    Bot(std::unique_ptr<IBotStrategy>&& strategy, unsigned roomId) 
        : strategy(std::move(strategy)), roomId(roomId) {}
    bool isPlayer() {return false;}
    std::string getName() override {return "Bot" + roomId;}
};