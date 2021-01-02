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
#include "rollout.h"

#include <cassert>
#include <random>

namespace mcts {

/**
 * @brief Policy performing a random move as default rollout policy
 */
class RandomPolicy
{
  public:
    template <class ProblemType>
    typename ProblemType::ActionType getAction(const typename ProblemType::StateType& state, const ProblemType& problem)
    {
        assert(problem.getNextStageType(state) == mcts::StageType::DECISION);
        auto actions = problem.getAvailableActions(state);
        auto ac = actions[engine_() % actions.size()];
        return ac;
    }

    void seed(size_t seed) { engine_.seed(seed); }

  private:
    std::minstd_rand0 engine_{std::random_device{}()};
};

using RandomRolloutPolicy = RolloutPolicy<RandomPolicy>;
}  // namespace mcts