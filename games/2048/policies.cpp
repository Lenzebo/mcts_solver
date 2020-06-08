#include "policies.h"

#include <algorithm>

namespace g2048 {
g2048::Actions FixedSequencePolicy::getAction(const mcts::MaxSizeVector<g2048::Actions, 4>& availableActions) const
{
    for (const auto action : ordering)
    {
        if (std::find(availableActions.begin(), availableActions.end(), action) != availableActions.end())
        {
            return action;
        }
    }
    assert(false);
    return {};
}

Actions FixedSequencePolicy::getAction(const g2048::G2048State& state, const g2048::G2048Problem& problem) const

{
    auto actions = problem.getAvailableActions(state);
    return getAction(actions);
}
}