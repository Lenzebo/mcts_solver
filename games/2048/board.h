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

#include <array>
#include <ostream>

namespace g2048 {

constexpr size_t BOARD_DIMS = 4;
constexpr size_t NUM_CELLS = BOARD_DIMS * BOARD_DIMS;

struct Board
{
    Board() = default;
    Board(std::array<std::array<uint8_t, BOARD_DIMS>, BOARD_DIMS> cells);

    [[nodiscard]] uint8_t at(size_t x, size_t y) const;
    void set(size_t x, size_t y, uint8_t value);
    void transpose();

    bool operator==(const Board& other) const { return values_ == other.values_; }

    [[nodiscard]] size_t numEmpty() const;
    [[nodiscard]] size_t biggestTile() const;
    [[nodiscard]] uint8_t biggestExp() const;

    [[nodiscard]] uint64_t raw() const { return values_; }
    friend std::ostream& operator<<(std::ostream& os, const Board& board);

  private:
    uint64_t values_;
};
}  // namespace g2048