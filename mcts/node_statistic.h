#pragma once

#include "types.h"
#include <array>

namespace mcts {

/**
 * This class holds all statistics for each discrete choices.
 * This could be either actions, events, edges, etc. Something with integer IDs
 * :) Unfortunately, it is the callers responsibility not to mix different ids.
 */
template <typename ValueType, int MAX_NUM_STATISTICS>
class NodeStatistic
{
  public:
    /// this struct hold all statistic for one action
    struct Statistic
    {
        ValueType total_value = 0.0;
        uint32_t count = 0;
        ValueType max_value = std::numeric_limits<ValueType>::lowest();
        ValueType value() const { return total_value / double(std::max(1U, count)); }
    };

    NodeStatistic() { statistics.fill(Statistic()); };

    [[nodiscard]] uint32_t getTotalVisits() const { return visit_count; }

    void initializeValue(size_t idx)
    {
        statistics[idx].total_value = 0;
        statistics[idx].count = 0;
        statistics[idx].max_value = std::numeric_limits<ValueType>::lowest();
    }

    /// this function allows to add a penalty to an action
    void addPenalty(size_t idx, const ValueType& penalty) { statistics[idx].total_value += penalty; }

    void visitWithValue(size_t idx, const ValueType& val)
    {
        statistics[idx].total_value += val;
        statistics[idx].count++;
        statistics[idx].max_value = std::max(statistics[idx].max_value, val);
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

    const Statistic& stat(size_t idx) const { return statistics[idx]; }

    // Returns the maximum and minimum of all the visits
    void getMinMaxValue(ValueType& max, ValueType& min) const
    {
        max = _max;
        min = _min;
    }

    const std::array<Statistic, MAX_NUM_STATISTICS>& getStatistics() const { return statistics; }

    const Statistic* begin() const { return statistics.begin(); }
    const Statistic* end() const { return statistics.end(); }

  private:
    std::array<Statistic, MAX_NUM_STATISTICS> statistics;
    uint32_t visit_count = 0;
    ValueType _min = 0.0;
    ValueType _max = 1.0;
};

}  // namespace mcts
