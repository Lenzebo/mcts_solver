#pragma once
#include <cmath>
#include <variant>
#include "../solver.h"

#include "selection.h"

#include <random>

namespace mcts {
template <typename ValueType>
class UCB1SelectionPolicy : public SelectionPolicy<UCB1SelectionPolicy<ValueType>>
{
  public:
    static_assert(std::is_arithmetic_v<ValueType>, "Value must be an arithmetic type");

    struct Parameter
    {
        ValueType min{0};
        ValueType max{1};
        float explorationConstant{5};
    };

    UCB1SelectionPolicy() = default;
    UCB1SelectionPolicy(const Parameter& params) : params_(params) {}

    template <typename Node>
    size_t selectDecisionNodeSuccessor(const Node&, const typename Node::DecisionNode& decision)
    {  // find best child based on UCT
        float best_uct_value = std::numeric_limits<float>::lowest();

        const auto& map = decision.statistics;
        size_t best_child = std::numeric_limits<size_t>::max();
        size_t index = 0;
        for (const auto& m : map)
        {
            if (m.count == 0)
            {
                index++;
                continue;
            }
            const uint32_t child_visits = std::max(1U, m.count);
            const float current_value = m.value();

            const float scaledValue = (current_value - params_.min) / (params_.max - params_.min);
            const uint32_t node_visits = std::max(1U, map.getTotalVisits());

            float current_UTC =
                scaledValue + params_.explorationConstant * sqrt(2 * logf(node_visits) / float(child_visits));

            if (current_UTC > best_uct_value)
            {
                best_uct_value = current_UTC;
                best_child = index;
            }
            index++;
        }
        return best_child;
    }

    Parameter params_{};
};
}