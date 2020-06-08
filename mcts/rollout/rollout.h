#pragma once

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

  protected:
    size_t rolloutDepth_{std::numeric_limits<size_t>::max()};
    float discount_{1.0f};
    Policy policy_{};
};
}