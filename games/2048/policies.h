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

#include "2048.h"
#include "mcts/rollout/random_rollout.h"

namespace g2048 {
class FixedSequencePolicy
{
  public:
    static constexpr std::array<g2048::Actions, 4> ORDERING = {g2048::Actions::DOWN, g2048::Actions::RIGHT,
                                                               g2048::Actions::UP, g2048::Actions::LEFT};

    [[nodiscard]] Actions getAction(const g2048::G2048State& state, const g2048::G2048Problem& problem) const;

  private:
    [[nodiscard]] g2048::Actions getAction(const zbo::MaxSizeVector<g2048::Actions, 4>& availableActions) const;
};

class BestPositionPolicy
{
  public:
    [[nodiscard]] Actions getAction(const g2048::G2048State& state, const g2048::G2048Problem& problem) const;

  private:
    struct Point
    {
        uint8_t x;
        uint8_t y;
    };
    std::vector<std::vector<Point>> paths_;

    [[nodiscard]] float getPositionalScore(const g2048::G2048State& state) const;
    [[nodiscard]] float getOrderingScore(const g2048::G2048State& state) const;

    [[nodiscard]] float getDifferenceScore(const g2048::G2048State& state) const;
    [[nodiscard]] float getEmptyFieldsScore(const g2048::G2048State& state) const;
    [[nodiscard]] float getCornerHighestScore(const g2048::G2048State& state) const;
    [[nodiscard]] float getOrderingScoreAlongPath(const g2048::G2048State& state, const std::vector<Point>& path) const;
};

template <class Solver>
class MCTSPolicy
{
  public:
    MCTSPolicy(Solver& solver) : solver_(&solver) {}
    [[nodiscard]] Actions getAction(const g2048::G2048State& state, const g2048::G2048Problem& problem) const
    {
        // try to find state in existing tree
        const auto& tree = solver_->tree();
        auto node = std::find_if(tree.begin(), tree.end(), [&state](const auto& node) { return node.state == state; });
        if (node != tree.end())
        {
            auto action = solver_->runFromExistingTree(node->nodeId);
            //            solver_->printTopLevelUtilities();
            return action;
        }
        return solver_->run(problem, state);
    }

  private:
    Solver* solver_;
};

class MCRolloutPolicy : public mcts::RandomRolloutPolicy
{
  public:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    MCRolloutPolicy(size_t iterations = 100, size_t depth = 100, const float discount = 0.99f)
        : mcts::RandomRolloutPolicy(depth, discount)
    {
        iterations_ = iterations;
    }

    [[nodiscard]] float rolloutMultiple(const g2048::G2048State state, const g2048::G2048Problem& game)
    {
        float avg = 0;
        for (size_t i = 0; i < iterations_; ++i) { avg += rollout(state, game); }
        return avg / iterations_;
    }

    [[nodiscard]] g2048::Actions getAction(g2048::G2048State state, const g2048::G2048Problem& game)
    {
        auto actions = game.getAvailableActions(state);
        g2048::Actions bestAction = actions[0];
        float bestValue = 0;

        for (auto action : actions)
        {
            g2048::G2048State cs = state;
            auto val = game.performAction(action, cs);
            if (!cs.isChanceNext())
            {
                // action didn't do anything, so we skip it
                continue;
            }
            float value = val + rolloutMultiple(cs, game);
            if (value > bestValue)
            {
                bestAction = action;
                bestValue = value;
            }
        }
        return bestAction;
    }

  private:
    size_t iterations_;
};
}  // namespace g2048