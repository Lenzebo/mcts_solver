#pragma once

#include "types.h"
#include "utils/named_type.h"

#include <cstdint>
#include <array>
#include <string>

namespace mcts {

template <int NUM_PLAYERS, int MAX_NUM_ACTIONS, typename STATE_TYPE, typename ACTION_TYPE = uint8_t,
          typename VALUE_TYPE = float, int MAX_CHANCE_EVENTS = 0, typename CHANCE_TYPE = uint8_t>
class Problem
{
  public:
    using ValueType = VALUE_TYPE;
    using ActionType = ACTION_TYPE;
    using ChanceEventType = CHANCE_TYPE;
    using StateType = STATE_TYPE;

    static constexpr int numPlayers = NUM_PLAYERS;
    static constexpr int maxNumActions = MAX_NUM_ACTIONS;
    static constexpr int maxChanceEvents = MAX_CHANCE_EVENTS;

    // derived quantities
    static_assert(MAX_NUM_ACTIONS > 1, "Number of actions must be > 1 so that we can plan with this problem ");
    static constexpr bool hasChanceEvents = MAX_CHANCE_EVENTS > 0;
    using ValueVector = std::array<ValueType, NUM_PLAYERS>;


    [[nodiscard]] ActionId actionToId(const StateType& state, const ActionType& action) const
    {
        (void)state;
        return ActionId{size_t(action)};
    }

    [[nodiscard]] ActionType idToAction(const StateType& state, const ActionId& action) const
    {
        (void)state;
        return static_cast<ActionType>(action.get());
    }

    [[nodiscard]] std::string actionToString(const StateType& state, const ActionType& action) const
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
            return NUM_PLAYERS - 1u;
        }
        else
        {
            return other_player_id - 1;
        }
    }

  private:
};
}  // namespace mcts
