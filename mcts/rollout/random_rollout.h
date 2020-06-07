#pragma once
#include "rollout.h"

namespace mcts {

/**
 * @brief Policy performing a
 */
class RandomPolicy
{
  public:
    template <class ProblemType>
    typename ProblemType::ValueVector performAction(typename ProblemType::StateType& state,
                                                    const ProblemType& problem) const
    {
        assert(problem.getNextStageType(state) == mcts::StageType::DECISION);
        return problem.performRandomAction(state);
    }
};

using RandomRolloutPolicy = RolloutPolicy<RandomPolicy>;
}