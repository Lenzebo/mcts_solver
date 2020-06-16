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

    size_t numEmpty() const;
    size_t biggestTile() const;
    uint8_t biggestExp() const;

    uint64_t raw() const { return values_; }
    friend std::ostream& operator<<(std::ostream& os, const Board& board);

  private:
    uint64_t values_;
};
}