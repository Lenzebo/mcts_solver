#pragma once
#include "mcts/utils/max_size_vector.h"

namespace mcts::detail {
template <int NumChance, class ProblemType, class ProblemDefinition>
class NeedsChanceEvents
{
  public:
    static constexpr bool hasChanceEvents = true;
    using ChanceEventsVector =
        mcts::MaxSizeVector<std::pair<float, typename ProblemDefinition::ChanceEventType>, NumChance>;

    /// Functions that should be implemented by the child classes. If you get a linker error pointing to here, use this
    /// signature and implement in the child class
    [[nodiscard]] typename ProblemDefinition::ValueVector performChanceEvent(
        const typename ProblemDefinition::ChanceEventType event, typename ProblemDefinition::StateType& state) const;
    [[nodiscard]] ChanceEventsVector getAvailableChanceEvents(const typename ProblemDefinition::StateType& state) const;
    [[nodiscard]] typename ProblemDefinition::ValueVector performRandomChanceEvent(
        typename ProblemDefinition::StateType& state) const;
};

// empty if no chance
template <class ProblemType, class ProblemDefinition>
class NeedsChanceEvents<0, ProblemType, ProblemDefinition>
{
  public:
    static constexpr bool hasChanceEvents = false;
};

template <class ProblemType, class ProblemDefinition>
class NeedActionFunction
{
  public:
    using ActionsVector = mcts::MaxSizeVector<typename ProblemDefinition::ActionType, ProblemDefinition::maxNumActions>;
    /// Functions that should be implemented by the child classes. If you get a linker error pointing to here, use this
    /// signature and implement in the child class
    [[nodiscard]] ActionsVector getAvailableActions(const typename ProblemDefinition::StateType& state) const;
    [[nodiscard]] typename ProblemDefinition::ValueVector performAction(
        const typename ProblemDefinition::ActionType action, typename ProblemDefinition::StateType& state) const;
};

}