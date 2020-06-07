#include "policies.h"

#include <algorithm>

namespace g2048 {
g2048::Actions FixedSequencePolicy::getAction(const mcts::MaxSizeVector<g2048::Actions, 4>& availableActions) const
{
    constexpr std::array<g2048::Actions, 4> ordering = {g2048::Actions::DOWN, g2048::Actions::RIGHT, g2048::Actions::UP,
                                                        g2048::Actions::LEFT};

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

float FixedSequencePolicy::performAction(g2048::G2048State& state, const g2048::G2048Problem& problem) const
{
    auto actions = problem.getAvailableActions(state);
    auto action = getAction(actions);
    return problem.performAction(action, state);
}
}