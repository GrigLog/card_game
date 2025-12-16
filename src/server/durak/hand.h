#pragma once

#include <vector>
#include "card.h"
#include <string>
#include <sstream>

namespace Hand {
    inline std::string ToString(const std::vector<TCard>& hand) {
        std::ostringstream oss;
        for (int i = 0; i < hand.size(); i++) {
            if (i > 0) {
                oss << ", ";
            }
            oss << hand[i].ToString();
        }
        return oss.str();
    }
} // namespace Hand