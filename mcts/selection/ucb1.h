#pragma once
#include <cmath>
#include <variant>
#include "../solver.h"

#include "selection.h"

#include <random>

namespace mcts {

/**
 * @brief Find best child based on UCT
 * @tparam ValueType
 */
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

    // NOLINTNEXTLINE
    UCB1SelectionPolicy(const Parameter& params) : params_(params) {}

    template <typename Node>
    size_t selectDecisionNodeSuccessor(const Node&, const typename Node::DecisionNode& decision)
    {
        float bestUCTValue = std::numeric_limits<float>::lowest();

        const auto& map = decision.statistics;
        size_t bestChild = std::numeric_limits<size_t>::max();
        size_t index = 0;

        const float currExplorationConstant = (params_.max - params_.min) * params_.explorationConstant;
        const uint32_t nodeVisits = map.getTotalVisits();
        const auto logVisits = logf(nodeVisits);

        for (const auto& m : map)
        {
            index++;

            if (!m.visited())
            {
                continue;
            }

            float currentUTC = m.value() + currExplorationConstant * sqrt(2 * logVisits / float(m.count()));

            if (currentUTC > bestUCTValue)
            {
                bestUCTValue = currentUTC;
                bestChild = index - 1;
            }
        }
        return bestChild;
    }

    Parameter params_{};
};
}