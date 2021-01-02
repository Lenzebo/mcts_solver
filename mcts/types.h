// MIT License
//
// Copyright (c) 2020 Lenzebo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "zbo/named_type.h"

#include <array>
#include <cstdint>

namespace mcts {
enum class StageType
{
    DECISION,
    CHANCE
};

struct ActionId : public zbo::NamedType<uint8_t, ActionId>, zbo::EqualityComparable<ActionId>
{
    using NamedType::NamedType;
};

struct ChanceEventId : public zbo::NamedType<uint8_t, ChanceEventId>, zbo::EqualityComparable<ChanceEventId>
{
    using NamedType::NamedType;
};

struct NoEvent
{
};

template <typename T, size_t n>
std::array<T, n> operator+(const std::array<T, n>& a1, const std::array<T, n>& a2)
{
    std::array<T, n> retval{};
    std::transform(a1.begin(), a1.end(), a2.begin(), retval.begin(), std::plus{});
    return retval;
}

template <typename T, size_t n>
std::array<T, n> operator*(const float f, const std::array<T, n>& a)
{
    std::array<T, n> retval{};
    std::transform(a.begin(), a.end(), retval.begin(), [f](const T val) { return f * val; });
    return retval;
}

}  // namespace mcts