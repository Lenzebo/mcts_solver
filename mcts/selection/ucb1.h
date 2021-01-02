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
#include "../solver.h"
#include "selection.h"

#include <cmath>
#include <random>
#include <variant>
#include <cassert>

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

            assert(!std::isnan(currentUTC));

            if (currentUTC > bestUCTValue)
            {
                bestUCTValue = currentUTC;
                bestChild = index - 1;
            }
        }
        assert(bestChild != std::numeric_limits<size_t>::max());
        return bestChild;
    }

  private:
    Parameter params_{};
};
}  // namespace mcts