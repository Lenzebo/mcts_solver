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

#include "mcts/types.h"

namespace mcts {

/**
 * @brief Class performing a rollout with a given depth (and a given discount factor) using the given (templated) policy
 * @tparam Policy policy to use. has to implement a getAction(state,problem) -> Action function in order to be called
 */
template <typename Policy>
class RolloutPolicy
{
  public:
    RolloutPolicy() = default;
    explicit RolloutPolicy(size_t depth, float discount = 1.0f) : rolloutDepth_(depth), discount_(discount){};
    RolloutPolicy(Policy&& policy) : policy_(std::move(policy)){};

    template <class ProblemType>
    typename ProblemType::ValueVector rollout(typename ProblemType::StateType state, const ProblemType& problem)
    {
        typename ProblemType::ValueVector retval{};
        size_t depth = 0;
        float currDiscount = 1;
        while (!problem.isTerminal(state))
        {
            switch (problem.getNextStageType(state))
            {
                case mcts::StageType::DECISION:
                {
                    auto action = policy_.getAction(state, problem);
                    auto reward = problem.performAction(action, state);
                    retval = retval + currDiscount * reward;
                    break;
                }
                case mcts::StageType::CHANCE:
                {
                    if constexpr (ProblemType::hasChanceEvents)
                    {
                        auto reward = problem.performRandomChanceEvent(state);
                        retval = retval + currDiscount * reward;
                    }
                    break;
                }
            }
            depth++;
            currDiscount *= discount_;
            if (depth > rolloutDepth_)
            {
                break;
            }
        }
        return retval;
    }

  private:
    size_t rolloutDepth_{std::numeric_limits<size_t>::max()};
    float discount_{1.0f};
    Policy policy_{};
};
}  // namespace mcts