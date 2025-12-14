#pragma once

#include <iostream>
#include <memory>

struct IBotStrategy {
    static std::unique_ptr<IBotStrategy> parse(std::istream& iss);
};

struct RandomStrategy : IBotStrategy {

};

struct SortedStrategy : IBotStrategy {
    float coeff;
    SortedStrategy(float coeff) : coeff(coeff) {}
};