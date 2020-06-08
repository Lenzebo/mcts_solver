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

    [[nodiscard]] ActionId actionToId(const typename ProblemDefinition::StateType& state,
                                      const typename ProblemDefinition::ActionType& action) const
    {
        (void)state;
        return ActionId{size_t(action)};
    }

    [[nodiscard]] typename ProblemDefinition::ActionType idToAction(const typename ProblemDefinition::StateType& state,
                                                                    const ActionId& action) const
    {
        (void)state;
        return static_cast<typename ProblemDefinition::ActionType>(action.get());
    }

    [[nodiscard]] std::string actionToString(const typename ProblemDefinition::StateType& state,
                                             const typename ProblemDefinition::ActionType& action) const
    {
        (void)state;
        using std::to_string;
        return to_string(action);
    }

    /**
     * Returns the ID of the player that acts before 'other_player_id'
     */
    [[nodiscard]] static uint8_t getPlayerBefore(uint8_t other_player_id)
    {
        if (other_player_id == 0)
        {
            return ProblemType::numPlayers - 1u;
        }
        else
        {
            return other_player_id - 1;
        }
    }

  private:
};
}  // namespace mcts
