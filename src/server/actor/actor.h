#pragma once

struct IActor {
    virtual bool isPlayer() = 0;
    virtual bool isOwner() {return false;};
    virtual std::string getName() = 0;
};