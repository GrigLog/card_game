#include "strategy.h"
#include <string>

std::unique_ptr<IBotStrategy> IBotStrategy::parse(std::istream& iss) {
    std::string name;
    if (iss >> name) {
        if (name == "random") {
            return std::make_unique<RandomStrategy>();
        } else if (name == "sorted") {
            float coeff;
            if (iss >> coeff) {
                if (coeff < 0)
                    coeff = 0;
                if (coeff > 1)
                    coeff = 1;
                return std::make_unique<SortedStrategy>(coeff);
            }
        }
    }
    return nullptr;
}