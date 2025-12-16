#pragma once

#include <string>

constexpr unsigned short PORT = 6969;

// Utility to allow overloading lambdas for use in std::visit
template <class... Ts>
struct TVisitOverloadUtility: Ts... {
    using Ts::operator()...;
};

using TResult = std::pair<bool, std::string>;