#pragma once

#include <string>
#include <format>

constexpr unsigned short PORT = 6969;

// Utility to allow overloading lambdas for use in std::visit
template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};