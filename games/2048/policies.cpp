#include "policies.h"

#include <algorithm>

namespace g2048 {
g2048::Actions FixedSequencePolicy::getAction(const zbo::MaxSizeVector<g2048::Actions, 4>& availableActions) const
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
Actions BestPositionPolicy::getAction(const g2048::G2048State& state, const g2048::G2048Problem& problem) const
{
    auto actions = problem.getAvailableActions(state);

    float bestScore = std::numeric_limits<float>::lowest();
    g2048::Actions bestAction{};

    for (auto action : actions)
    {
        auto currState = state;
        problem.performAction(action, currState);
        float score = getPositionalScore(currState);
        if (score > bestScore)
        {
            bestAction = action;
            bestScore = score;
        }
    }

    return bestAction;
}

float BestPositionPolicy::getPositionalScore(const g2048::G2048State& state) const
{
    const float wDiff = 10.0f;
    const float wOrder = 5.0f;
    const float wEmpty = 1.0f;
    const float wCorner = 100.0f;
    return wDiff * getDifferenceScore(state) + wOrder * getOrderingScore(state) + wEmpty * getEmptyFieldsScore(state) +
           wCorner * getCornerHighestScore(state);
}

float BestPositionPolicy::getOrderingScore([[maybe_unused]] const g2048::G2048State& state) const
{
    return 0;
}

float BestPositionPolicy::getDifferenceScore(const g2048::G2048State& state) const
{
    float value = 0;
    for (uint8_t x = 0; x < BOARD_DIMS - 1; x++)
    {
        for (uint8_t y = 0; y < BOARD_DIMS - 1; y++)
        {
            value += 2.0f * float(state.board(x, y)) - float(state.board(x + 1, y)) - float(state.board(x, y + 1));
        }
    }
    return -value;
}
float BestPositionPolicy::getEmptyFieldsScore(const g2048::G2048State& state) const
{
    return float(state.board().numEmpty());
}
float BestPositionPolicy::getOrderingScoreAlongPath(const g2048::G2048State& state,
                                                    const std::vector<Point>& path) const
{
    float score = 0;
    constexpr float ratio = 0.9;
    float currRatio = 1.0f;

    for (auto pt : path)
    {
        score += float(state.board(pt.x, pt.y)) * currRatio;
        currRatio *= ratio;
    }
    return score;
}

float BestPositionPolicy::getCornerHighestScore(const g2048::G2048State& state) const
{
    auto maxScore = float(state.board().biggestExp());
    float maxError = 19;

    maxError = std::min(maxError, maxScore - float(state.board(0, 0)));
    maxError = std::min(maxError, maxScore - float(state.board(0, BOARD_DIMS - 1)));
    maxError = std::min(maxError, maxScore - float(state.board(BOARD_DIMS - 1, 0)));
    maxError = std::min(maxError, maxScore - float(state.board(BOARD_DIMS - 1, BOARD_DIMS - 1)));

    return -float(maxError);
}

}  // namespace g2048