#pragma once

#include <iostream>
#include <memory>
#include "../command.h"
#include "../durak/card.h"

struct DurakGame;

struct IBotStrategy {
    static std::unique_ptr<IBotStrategy> parse(std::istream& iss);

    virtual GameCommand generateCommand(const DurakGame& game);

protected:
    //Возвращает -1, если не хочет выбирать, и число от 0 до hand.size() иначе
    virtual int selectFromAvailable(const std::vector<Card>& hand, Suit trump) {return -1;}
};


struct RandomStrategy : IBotStrategy {
protected:
    virtual int selectFromAvailable(const std::vector<Card>& hand, Suit trump) override;
};


struct SortedStrategy : IBotStrategy {
    float coeff;
    SortedStrategy(float coeff) : coeff(coeff) {}
protected:
    virtual int selectFromAvailable(const std::vector<Card>& hand, Suit trump) override;
private:
    static int calculatePriority(const Card& card, Suit trump);
};


//This should never fail, in theory.
struct FallbackStrategy : IBotStrategy {
    virtual GameCommand generateCommand(const DurakGame& game) override;
};