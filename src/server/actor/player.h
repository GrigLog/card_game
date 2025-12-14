#pragma once
#include "actor.h"

struct Player : IActor {
    unsigned id;
    bool bOwner;

    Player(unsigned id, bool bOwner) : id(id), bOwner(bOwner) {}
    bool isPlayer() override {return true;}
    bool isOwner() override {return bOwner; }
    std::string getName() override {return "Player" + id;}
};