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

#include "mcts/details/problem_impl.h"
#include "types.h"
#include "zbo/max_size_vector.h"
#include "zbo/named_type.h"

#include <array>
#include <cstdint>
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
