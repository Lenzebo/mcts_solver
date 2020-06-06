#pragma once
#include <cmath>
#include <variant>
#include "../solver.h"

#include <random>

namespace mcts {
template <typename ValueType>
class UCB1SelectionPolicy
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
    size_t selectSuccessor(const Node& currentNode)
    {
        return currentNode.visit(*this);
    }

    template <typename Node>
    size_t operator()(const Node&, const typename Node::ChanceNode& chance) const
    {
        static thread_local std::minstd_rand0 engine{};
        static thread_local std::uniform_real_distribution<float> dist{0, 1.0f};

        float randVal = dist(engine);

        size_t counter = 0;
        for (const auto& ev : chance.remainingEvents)
        {
            if (randVal < ev.first)
            {
                return counter;
            }
            randVal -= ev.first;
            counter++;
        }
        return chance.remainingEvents.size() - 1;
    }

    template <typename Node>
    size_t operator()(const Node&, const typename Node::DecisionNode& decision) const
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