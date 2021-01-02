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
#include "zbo/max_size_vector.h"

namespace mcts::detail {
template <int numChance, class ProblemType, class ProblemDefinition>
class NeedsChanceEvents
{
  public:
    static constexpr bool HAS_CHANCE_EVENTS = true;
    using ChanceEventsVector =
        zbo::MaxSizeVector<std::pair<float, typename ProblemDefinition::ChanceEventType>, numChance>;

  private:
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
    static constexpr bool HAS_CHANCE_EVENTS = false;  // NOLINT(readability-identifier-naming)
};

template <class ProblemType, class ProblemDefinition>
class NeedActionFunction
{
  public:
    using ActionsVector =
        zbo::MaxSizeVector<typename ProblemDefinition::ActionType, ProblemDefinition::MAX_NUM_ACTIONS>;

  private:
    /// Functions that should be implemented by the child classes. If you get a linker error pointing to here, use this
    /// signature and implement in the child class
    [[nodiscard]] ActionsVector getAvailableActions(const typename ProblemDefinition::StateType& state) const;
    [[nodiscard]] typename ProblemDefinition::ValueVector performAction(
        const typename ProblemDefinition::ActionType action, typename ProblemDefinition::StateType& state) const;
};

}  // namespace mcts::detail