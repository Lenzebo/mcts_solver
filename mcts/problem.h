#pragma once

#include "types.h"
#include "utils/named_type.h"
#include "utils/max_size_vector.h"
#include "mcts/details/problem_impl.h"

#include <cstdint>
#include <array>
#include <string>

namespace mcts {

template <typename ProblemType, typename ProblemDefinition>
class Problem : public ProblemDefinition,  // To make all typedefs available
                public detail::NeedsChanceEvents<ProblemDefinition::maxChanceEvents, ProblemType,
                                                 ProblemDefinition>,  // to make sure that all chance events methods are
                                                                      // implemented
                public detail::NeedActionFunction<ProblemType, ProblemDefinition>  // to make sure that all action
                                                                                   // perform methods are implemented
{
  public:
    // checks to catch errors in ProblemDefinition early
    static_assert(ProblemDefinition::maxNumActions > 1,
                  "Number of actions must be > 1 so that we can plan with this problem ");
    static_assert(std::is_arithmetic_v<typename ProblemDefinition::ValueType>, "Valuetype must be arithmetic");

    [[nodiscard]] ActionId actionToId([[maybe_unused]] const typename ProblemDefinition::StateType& state,
                                      const typename ProblemDefinition::ActionType& action) const
    {
        return ActionId{size_t(action)};
    }

    [[nodiscard]] typename ProblemDefinition::ActionType idToAction(
        [[maybe_unused]] const typename ProblemDefinition::StateType& state, const ActionId& action) const
    {
        return static_cast<typename ProblemDefinition::ActionType>(action.get());
    }

    [[nodiscard]] std::string actionToString([[maybe_unused]] const typename ProblemDefinition::StateType& state,
                                             const typename ProblemDefinition::ActionType& action) const
    {
        using std::to_string;
        return to_string(action);
    }
    [[nodiscard]] std::string eventToString([[maybe_unused]] const typename ProblemDefinition::StateType& state,
                                            const typename ProblemDefinition::ChanceEventType& event) const
    {
        using std::to_string;
        return to_string(event);
    }

  private:
};
}  // namespace mcts
