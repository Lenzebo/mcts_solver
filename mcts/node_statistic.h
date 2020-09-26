#pragma once

#include "types.h"
#include <array>

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
template <typename ValueType, int MAX_NUM_STATISTICS>
class NodeStatistic
{
  public:
    using Stat = Statistic<ValueType>;
    NodeStatistic() { statistics.fill(Stat()); };

    [[nodiscard]] uint32_t getTotalVisits() const { return visit_count; }

    void initializeValue(size_t idx)
    {
        statistics[idx].total_value = 0;
        statistics[idx].count = 0;
        statistics[idx].max_value = std::numeric_limits<ValueType>::lowest();
    }

    void visitWithValue(size_t idx, const ValueType& val)
    {
        statistics[idx].add(val);
        visit_count++;

        if (visit_count == 1)
        {
            _min = _max = val;
        }
        else
        {
            _min = std::min(_min, val);
            _max = std::max(_max, val);
        }
    }

    const Stat& stat(size_t idx) const { return statistics[idx]; }

    // Returns the maximum and minimum of all the visits
    void getMinMaxValue(ValueType& max, ValueType& min) const
    {
        max = _max;
        min = _min;
    }

    const std::array<Stat, MAX_NUM_STATISTICS>& getStatistics() const { return statistics; }

    const Stat* begin() const { return statistics.begin(); }
    const Stat* end() const { return statistics.end(); }

  private:
    std::array<Stat, MAX_NUM_STATISTICS> statistics;
    uint32_t visit_count = 0;
    ValueType _min = 0.0;
    ValueType _max = 1.0;
};

}  // namespace mcts
