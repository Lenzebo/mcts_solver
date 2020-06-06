#pragma once

namespace mcts {

class RandomRolloutPolicy
{
  public:
    template <class ProblemType>
    typename ProblemType::ValueVector rollout(typename ProblemType::StateType state, const ProblemType& problem) const
    {
        typename ProblemType::ValueVector retval{};
        size_t depth = 0;
        while (!problem.isTerminal(state))
        {
            if constexpr (ProblemType::hasChanceEvents)
            {
                switch (problem.getNextStageType(state))
                {
                    case mcts::StageType::DECISION:
                    {
                        auto reward = problem.performRandomAction(state, state);
                        retval = retval + reward;
                        break;
                    }
                    case mcts::StageType::CHANCE:
                    {
                        auto reward = problem.performRandomChanceEvent(state);
                        retval = retval + reward;
                        break;
                    }
                }
            }
            else
            {
                auto reward = problem.performRandomAction(state, state);
                retval = retval + reward;
            }

            depth++;
            if (depth > rolloutDepth_)
            {
                break;
            }
        }
        return retval;
    }

  private:
    size_t rolloutDepth_{std::numeric_limits<size_t>::max()};
};

}