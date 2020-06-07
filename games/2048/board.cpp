#include "board.h"

#include <iomanip>

namespace g2048 {

// TODO for a better bit level magic, look at https://github.com/nneonneo/2048-ai/blob/master/2048.cpp

constexpr uint8_t NIBBLE_MASK = 0xFU;
constexpr uint32_t BITS_PER_NIBBLE = 4;

inline uint64_t transpose(uint64_t x)
{
    uint64_t a1 = x & 0xF0F00F0FF0F00F0FULL;
    uint64_t a2 = x & 0x0000F0F00000F0F0ULL;
    uint64_t a3 = x & 0x0F0F00000F0F0000ULL;
    uint64_t a = a1 | (a2 << 12) | (a3 >> 12);
    uint64_t b1 = a & 0xFF00FF0000FF00FFULL;
    uint64_t b2 = a & 0x00FF00FF00000000ULL;
    uint64_t b3 = a & 0x00000000FF00FF00ULL;
    return b1 | (b2 >> 24) | (b3 << 24);
}

uint32_t position(size_t x, size_t y)
{
    return (4 * y + x);
}

uint32_t shift(size_t x, size_t y)
{
    return BITS_PER_NIBBLE * position(x, y);
}

void Board::transpose()
{
    values_ = g2048::transpose(values_);
}

size_t Board::numEmpty() const
{
    if (values_ == 0)
    {
        return 16;
    }

    auto x = values_;
    x |= (x >> 2U) & 0x3333333333333333ULL;
    x |= (x >> 1U);
    x = ~x & 0x1111111111111111ULL;
    // At this point each nibble is:
    //  0 if the original nibble was non-zero
    //  1 if the original nibble was zero
    // Next sum them all
    x += x >> 32U;
    x += x >> 16U;
    x += x >> 8U;
    x += x >> 4U;  // this can overflow to the next nibble if there were 16 empty positions
    return x & 0xfU;
}

size_t Board::biggestTile() const
{
    uint8_t max = 0;
    for (uint8_t y = 0; y < BOARD_DIMS; ++y)
    {
        for (uint8_t x = 0; x < BOARD_DIMS; ++x)
        {
            max = std::max(max, at(x, y));
        }
    }
    return 1U << max;
}

Board::Board(std::array<std::array<uint8_t, BOARD_DIMS>, BOARD_DIMS> cells)
{
    for (uint8_t y = 0; y < BOARD_DIMS; ++y)
    {
        for (uint8_t x = 0; x < BOARD_DIMS; ++x)
        {
            set(x, y, cells[x][y]);
        }
    }
}

uint8_t Board::at(size_t x, size_t y) const
{
    uint32_t shift = g2048::shift(x, y);
    auto shiftedToNibble = (values_ >> shift);
    auto retval = shiftedToNibble & NIBBLE_MASK;
    return retval;
}

void Board::set(size_t x, size_t y, uint8_t value)
{
    uint32_t shift = g2048::shift(x, y);
    uint64_t shiftedValue = uint64_t(value) << shift;
    uint64_t mask = uint64_t(NIBBLE_MASK) << shift;

    values_ = (values_ & ~mask) | shiftedValue;
}
std::ostream& operator<<(std::ostream& os, const Board& board)
{
    for (uint8_t y = 0; y < BOARD_DIMS; ++y)
    {
        for (uint8_t x = 0; x < BOARD_DIMS; ++x)
        {
            auto c = board.at(x, y);
            if (c > 0)
            {
                os << std::setw(5) << int(1U << board.at(x, y)) << " ";
            }
            else
            {
                os << std::setw(6) << "0 ";
            }
        }
        os << "\n";
    }
    return os;
}

}  // namespace g2048
