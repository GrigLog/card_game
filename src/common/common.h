#pragma once

#include <string>
#include <format>

constexpr unsigned short PORT = 6969;

// Utility to allow overloading lambdas for use in std::visit
template<class... Ts>
struct VisitOverloadUtility : Ts... {
    using Ts::operator()...;
};

using Result = std::pair<bool, std::string>;