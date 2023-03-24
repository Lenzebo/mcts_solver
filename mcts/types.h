#pragma once
#include "utils/named_type.h"

#include <cstdint>
#include <array>
#include <string>

namespace mcts {
enum class StageType
{
    DECISION,
    CHANCE
};

using ActionId = NamedType<uint8_t, struct ActionTag>;
using ChanceEventId = NamedType<uint8_t, struct ChangeEventTag>;

struct NoEvent
{
};

template <typename T, size_t N>
std::array<T, N> operator+(const std::array<T, N>& a1, const std::array<T, N>& a2)
{
    std::array<T, N> retval{};
    for (size_t i = 0; i < N; ++i)
    {
        retval[i] = a1[i] + a2[i];
    }
    return retval;
}

template <typename T, size_t N>
std::array<T, N> operator-(const std::array<T, N>& a1, const std::array<T, N>& a2)
{
    std::array<T, N> retval{};
    for (size_t i = 0; i < N; ++i)
    {
        retval[i] = a1[i] - a2[i];
    }
    return retval;
}

template <typename T, size_t N>
std::array<T, N>& operator-=(std::array<T, N>& a1, const std::array<T, N>& a2)
{
    for (size_t i = 0; i < N; ++i)
    {
        a1[i] -= a2[i];
    }
    return a1;
}

template <typename T, size_t N>
std::array<T, N> operator*(const float f, const std::array<T, N>& a)
{
    std::array<T, N> retval{};
    for (size_t i = 0; i < N; ++i)
    {
        retval[i] = f * a[i];
    }
    return retval;
}

}  // namespace mcts