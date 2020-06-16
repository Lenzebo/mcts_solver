#pragma once
#include "rollout.h"

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
}