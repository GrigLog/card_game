#pragma once

#include <iostream>
#include <memory>

#include "../command.h"
#include "../durak/card.h"

struct TDurakGame;

class IBotStrategy {
public:
    static std::unique_ptr<IBotStrategy> Parse(std::istream& iss);

    virtual TGameCommand GenerateCommand(const TDurakGame& game);

protected:
    // Возвращает -1, если не хочет выбирать, и число от 0 до hand.size() иначе
    virtual int SelectFromAvailable(const std::vector<TCard>& hand, ESuit trump) {
        return -1;
    }
};

class TRandomStrategy: public IBotStrategy {
protected:
    int SelectFromAvailable(const std::vector<TCard>& hand, ESuit trump) override;
};

class TSortedStrategy: public IBotStrategy {
public:
    float Coeff;
    TSortedStrategy(float coeff)
        : Coeff(coeff) 
    {}

protected:
    int SelectFromAvailable(const std::vector<TCard>& hand, ESuit trump) override;

private:
    static int CalculatePriority(const TCard& card, ESuit trump);
};

// This should never fail, in theory.
struct FallbackStrategy: public IBotStrategy {
    TGameCommand GenerateCommand(const TDurakGame& game) override;
};