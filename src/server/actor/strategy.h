#pragma once

#include <iostream>
#include <memory>
//#include "../durak/durak.h"

struct IBotStrategy {
    static std::unique_ptr<IBotStrategy> parse(std::istream& iss);

    //void reactToAttacked(DurakGame& game, );
};

struct RandomStrategy : IBotStrategy {

};

struct SortedStrategy : IBotStrategy {
    float coeff;
    SortedStrategy(float coeff) : coeff(coeff) {}
};