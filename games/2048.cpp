#include "2048.h"

namespace g2048 {

size_t G2048State::biggestTile() const
{
    uint8_t max = 0;
    for (uint8_t y = 0; y < BOARD_DIMS; ++y)
    {
        for (uint8_t x = 0; x < BOARD_DIMS; ++x)
        {
            max = std::max(max, board(x, y));
        }
    }
    return 1U << max;
}

std::ostream& G2048State::writeToStream(std::ostream& stream) const  // NOLINT
{
    for (uint8_t y = 0; y < BOARD_DIMS; ++y)
    {
        for (uint8_t x = 0; x < BOARD_DIMS; ++x)
        {
            auto c = board(x, y);
            if (c > 0)
            {
                stream << std::setw(5) << int(1U << board(x, y)) << " ";
            }
            else
            {
                stream << std::setw(6) << "0 ";
            }
        }
        stream << "\n";
    }
    return mcts::State<G2048State>::writeToStream(stream);
}

std::string G2048Problem::actionToString(const StateType& state, const ActionType& action) const  // NOLINT
{
    switch (action)
    {
        case UP:
            return "UP";
        case LEFT:
            return "LEFT";
        case RIGHT:
            return "RIGHT";
        case DOWN:
            return "DOWN";
    }
    return Problem::actionToString(state, action);
}
mcts::StageType G2048Problem::getNextStageType(const G2048State& state)
{
    if (state.isChanceNext())
    {
        return mcts::StageType::CHANCE;
    }
    else
    {
        return mcts::StageType::DECISION;
    }
}

/**
 * Should return a list of all possible actions for this player
 */
[[nodiscard]] mcts::MaxSizeVector<Actions, 4> G2048Problem::getAvailableActions(const G2048State& state) const
{
    if (state.isChanceNext())
    {
        return {};
    }

    // TODO(DLE) find out whether we can swipe in any direction
    mcts::MaxSizeVector<Actions, 4> retval{};
    if (canMoveUp(state))
    {
        retval.push_back(UP);
    }
    if (canMoveLeft(state))
    {
        retval.push_back(LEFT);
    }
    if (canMoveRight(state))
    {
        retval.push_back(RIGHT);
    }
    if (canMoveDown(state))
    {
        retval.push_back(DOWN);
    }
    return retval;
};

/**
 * Should return a list of all possible chance events with probability
 */
[[nodiscard]] mcts::MaxSizeVector<std::pair<float, ChanceEvent>, NUM_CELLS * 2> G2048Problem::getAvailableChanceEvents(
    const G2048State& state) const
{
    mcts::MaxSizeVector<std::pair<float, ChanceEvent>, NUM_CELLS * 2> retval;
    if (!state.isChanceNext())
    {
        return retval;
    }

    size_t numEmpty = countEmptyCells(state);

    const float probabilityPerCell = 1 / float(numEmpty);

    for (uint8_t x = 0; x < BOARD_DIMS; ++x)
    {
        for (uint8_t y = 0; y < BOARD_DIMS; ++y)
        {
            if (state.board(x, y) == 0)
            {
                retval.push_back(std::make_pair(probabilityPerCell * PROBABILITY_SPAWN_2, ChanceEvent{x, y, 1}));
                retval.push_back(std::make_pair(probabilityPerCell * PROBABILITY_SPAWN_4, ChanceEvent{x, y, 2}));
            }
        }
    }

    return retval;
};

/**
 * Apply the action to the gamestate. Gamestate will be changed with this
 */
G2048Problem::ValueVector G2048Problem::performChanceEvent(const ChanceEventType event, const G2048State& before,
                                                           G2048State& after) const
{
    assert(before.isChanceNext());
    if (&before != &after)
    {
        // if before and after point to different memory locations, we need to first copy the before state to after
        after = before;
    }
    after.setNextChance(false);
    assert(after.board(event.x, event.y) == 0);

    after.setBoard(event.x, event.y, event.value);

    return {};
}
/**
 * Apply the action to the gamestate. Gamestate will be changed with this
 */
G2048Problem::ValueVector G2048Problem::performAction(const ActionType action, const G2048State& before,
                                                      G2048State& after) const
{
    assert(!before.isChanceNext());

    ValueVector retval{0};
    if (&before != &after)
    {
        // if before and after point to different memory locations, we need to first copy the before state to after
        after = before;
    }

    int start = 0;
    int end = 0;
    int increment = 1;
    bool upDown = (action == UP || action == DOWN);
    switch (action)
    {
        case UP:
            start = 0;
            end = BOARD_DIMS;
            break;
        case LEFT:
            start = 0;
            end = BOARD_DIMS;
            break;
        case RIGHT:
            start = BOARD_DIMS - 1;
            increment = -1;
            end = -1;
            break;
        case DOWN:
            start = BOARD_DIMS - 1;
            increment = -1;
            end = -1;
            break;
    }
    size_t numUpdatedCells = 0;
    for (size_t otherdim = 0; otherdim < BOARD_DIMS; otherdim++)
    {
        for (int xy = start; xy != end; xy += increment)
        {
            const size_t x = upDown ? otherdim : xy;
            const size_t y = upDown ? xy : otherdim;
            bool changed = false;
            for (int xyd = xy + increment; xyd != end; xyd += increment)
            {
                const size_t xd = upDown ? otherdim : xyd;
                const size_t yd = upDown ? xyd : otherdim;

                if (after.board(xd, yd) == 0)
                {
                    continue;
                }

                if (after.board(x, y) == 0)
                {
                    after.setBoard(x, y, after.board(xd, yd));
                    numUpdatedCells++;
                    after.setBoard(xd, yd, 0);
                    numUpdatedCells++;
                    changed = true;
                    break;
                }
                else if (after.board(x, y) == after.board(xd, yd))
                {
                    after.setBoard(x, y, after.board(x, y) + 1);
                    numUpdatedCells++;
                    retval[0] += 1U << after.board(xd, yd);
                    after.setBoard(xd, yd, 0);
                    numUpdatedCells++;
                    break;
                }
                else
                {
                    break;
                }
            }
            if (changed)
            {
                xy -= increment;
            }
        }
    }

    const bool didChangeAnyCell = numUpdatedCells > 0;
    after.setNextChance(didChangeAnyCell);

    //        after.addRandomElement();
    return retval;
}

G2048Problem::ValueVector G2048Problem::performRandomChanceEvent(G2048State& state) const
{
    addRandomElement(state);
    return {};
}

G2048Problem::ValueVector G2048Problem::performRandomAction(const G2048State& before, G2048State& after) const
{
    auto actions = getAvailableActions(before);
    auto ac = actions[engine() % actions.size()];
    return performAction(ac, before, after);
}

[[nodiscard]] bool G2048Problem::isTerminal(const G2048State& state) const
{
    return !state.isChanceNext() && !canMoveLeft(state) && !canMoveRight(state) && !canMoveUp(state) &&
           !canMoveDown(state);
}

bool G2048Problem::canMoveLeft(const G2048State& state) const
{
    for (size_t x = 0; x < BOARD_DIMS - 1; ++x)
    {
        for (size_t y = 0; y < BOARD_DIMS; ++y)
        {
            auto c = state.board(x, y);
            auto cright = state.board(x + 1, y);
            if (cright > 0 && (c == 0 || c == cright))
            {
                return true;
            }
        }
    }
    return false;
}
bool G2048Problem::canMoveRight(const G2048State& state) const
{
    for (size_t x = 0; x < BOARD_DIMS - 1; ++x)
    {
        for (size_t y = 0; y < BOARD_DIMS; ++y)
        {
            auto c = state.board(x, y);
            auto cright = state.board(x + 1, y);
            if (c > 0 && (cright == 0 || c == cright))
            {
                return true;
            }
        }
    }
    return false;
}
bool G2048Problem::canMoveUp(const G2048State& state) const
{
    for (size_t y = 0; y < BOARD_DIMS - 1; ++y)
    {
        for (size_t x = 0; x < BOARD_DIMS; ++x)
        {
            auto c = state.board(x, y);
            auto cbelow = state.board(x, y + 1);
            if (cbelow > 0 && (c == 0 || c == cbelow))
            {
                return true;
            }
        }
    }
    return false;
}
bool G2048Problem::canMoveDown(const G2048State& state) const
{
    for (size_t y = 0; y < BOARD_DIMS - 1; ++y)
    {
        for (size_t x = 0; x < BOARD_DIMS; ++x)
        {
            auto c = state.board(x, y);
            auto cbelow = state.board(x, y + 1);
            if (c > 0 && (cbelow == 0 || c == cbelow))
            {
                return true;
            }
        }
    }
    return false;
}

[[nodiscard]] size_t G2048Problem::countEmptyCells(const G2048State& state) const
{
    size_t count = 0;
    for (uint8_t x = 0; x < BOARD_DIMS; ++x)
    {
        for (uint8_t y = 0; y < BOARD_DIMS; ++y)
        {
            auto cell = state.board(x, y);
            if (cell == 0)
            {
                count++;
            }
        }
    }
    return count;
}

void G2048Problem::addRandomElement(G2048State& state) const
{
    assert(state.isChanceNext());
    state.setNextChance(false);
    size_t numEmpty = countEmptyCells(state);
    assert(numEmpty > 0);

    std::uniform_int_distribution<size_t> dist(0, numEmpty - 1);
    size_t randNum = dist(engine);

    for (uint8_t x = 0; x < BOARD_DIMS; ++x)
    {
        for (uint8_t y = 0; y < BOARD_DIMS; ++y)
        {
            if (state.board(x, y) != 0)
            {
                continue;
            }
            if (randNum == 0)
            {
                if (bernoulli(engine))
                {
                    state.setBoard(x, y, 1);
                }
                else
                {
                    state.setBoard(x, y, 2);
                }
                return;
            }
            randNum--;
        }
    }
    assert(false);
}

}  // namespace g2048
