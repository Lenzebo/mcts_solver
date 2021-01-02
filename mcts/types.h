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