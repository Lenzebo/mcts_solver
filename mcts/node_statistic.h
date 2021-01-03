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

#include "types.h"

#include <array>
#include <limits>

namespace mcts {

/// this struct hold all statistic for one action
template <typename ValueType>
struct Statistic
{
    void add(const ValueType& value)
    {
        totalValue_ += value;
        count_++;
        maxValue_ = std::max(maxValue_, value);
    }

    [[nodiscard]] ValueType value() const noexcept { return totalValue_ / double(std::max(1U, count_)); }
    [[nodiscard]] uint32_t count() const noexcept { return count_; }
    [[nodiscard]] ValueType max() const noexcept { return maxValue_; }
    [[nodiscard]] bool visited() const noexcept { return count_ != 0; }

  private:
    ValueType totalValue_ = 0.0;
    uint32_t count_ = 0;
    ValueType maxValue_ = std::numeric_limits<ValueType>::lowest();
};

/**
 * This class holds all statistics for each discrete choices.
 * This could be either actions, events, edges, etc. Something with integer IDs
 * :) Unfortunately, it is the callers responsibility not to mix different ids.
 */
template <typename ValueType, int maxNumStatistics>
class NodeStatistic
{
  public:
    using Stat = Statistic<ValueType>;
    NodeStatistic() { statistics_.fill(Stat()); };

    [[nodiscard]] uint32_t getTotalVisits() const { return visitCount_; }

    void initializeValue(size_t idx)
    {
        statistics_[idx].total_value = 0;
        statistics_[idx].count = 0;
        statistics_[idx].max_value = std::numeric_limits<ValueType>::lowest();
    }

    void visitWithValue(size_t idx, const ValueType& val)
    {
        statistics_.at(idx).add(val);
        visitCount_++;

        if (visitCount_ == 1) { min_ = max_ = val; }
        else
        {
            min_ = std::min(min_, val);
            max_ = std::max(max_, val);
        }
    }

    [[nodiscard]] const Stat& stat(size_t idx) const { return statistics_[idx]; }

    // Returns the maximum and minimum of all the visits
    void getMinMaxValue(ValueType& max, ValueType& min) const
    {
        max = max_;
        min = min_;
    }

    [[nodiscard]] const std::array<Stat, maxNumStatistics>& getStatistics() const { return statistics_; }

    [[nodiscard]] const Stat* begin() const { return statistics_.begin(); }
    [[nodiscard]] const Stat* end() const { return statistics_.end(); }

  private:
    std::array<Stat, maxNumStatistics> statistics_;
    uint32_t visitCount_ = 0;
    ValueType min_ = 0.0;
    ValueType max_ = 1.0;
};

}  // namespace mcts
