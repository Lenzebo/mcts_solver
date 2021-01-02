#include "2048.h"

namespace g2048 {

std::ostream& G2048State::writeToStream(std::ostream& stream) const  // NOLINT(readability-identifier-naming)
{
    stream << board();
    return mcts::State<G2048State>::writeToStream(stream);
}
bool G2048State::empty(size_t x, size_t y) const
{
    auto value = 1 << board(x, y);
    return value == 1;
}
bool G2048State::operator==(const G2048State& rhs) const
{
    return board_ == rhs.board_ && nextIsChance == rhs.nextIsChance;
}
bool G2048State::operator!=(const G2048State& rhs) const
{
    return !(rhs == *this);
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
[[nodiscard]] zbo::MaxSizeVector<Actions, 4> G2048Problem::getAvailableActions(const G2048State& state) const
{
    if (state.isChanceNext())
    {
        return {};
    }

    zbo::MaxSizeVector<Actions, 4> retval{};
    auto can = canMove(state);
    if (can.up)
    {
        retval.push_back(UP);
    }
    if (can.left)
    {
        retval.push_back(LEFT);
    }
    if (can.right)
    {
        retval.push_back(RIGHT);
    }
    if (can.down)
    {
        retval.push_back(DOWN);
    }
    return retval;
};

/**
 * Should return a list of all possible chance events with probability
 */
[[nodiscard]] zbo::MaxSizeVector<std::pair<float, ChanceEvent>, NUM_CELLS * 2> G2048Problem::getAvailableChanceEvents(
    const G2048State& state) const
{
    zbo::MaxSizeVector<std::pair<float, ChanceEvent>, NUM_CELLS * 2> retval;
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
ProblemDefinition::ValueVector G2048Problem::performChanceEvent(const ChanceEventType event, G2048State& state) const
{
    assert(state.isChanceNext());
    state.setNextChance(false);
    assert(state.board(event.x, event.y) == 0);

    state.setBoard(event.x, event.y, event.value);

    return {};
}
/**
 * Apply the action to the gamestate. Gamestate will be changed with this
 */
ProblemDefinition::ValueVector G2048Problem::performAction(const ActionType action, G2048State& state) const
{
    assert(!state.isChanceNext());

    ValueVector retval{0};

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

                if (state.board(xd, yd) == 0)
                {
                    continue;
                }

                if (state.board(x, y) == 0)
                {
                    state.setBoard(x, y, state.board(xd, yd));
                    numUpdatedCells++;
                    state.setBoard(xd, yd, 0);
                    numUpdatedCells++;
                    changed = true;
                    break;
                }
                else if (state.board(x, y) == state.board(xd, yd))
                {
                    state.setBoard(x, y, state.board(x, y) + 1);
                    numUpdatedCells++;
                    retval += 1U << state.board(xd, yd);
                    state.setBoard(xd, yd, 0);
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
    state.setNextChance(didChangeAnyCell);
    return retval;
}

ProblemDefinition::ValueVector G2048Problem::performRandomChanceEvent(G2048State& state) const
{
    addRandomElement(state);
    return {};
}

ProblemDefinition::ValueVector G2048Problem::performRandomAction(G2048State& state) const
{
    auto actions = getAvailableActions(state);
    auto ac = actions[engine() % actions.size()];
    return performAction(ac, state);
}

[[nodiscard]] bool G2048Problem::isTerminal(const G2048State& state) const
{
    auto can = canMove(state);
    return !state.isChanceNext() && !can.any();
}

G2048Problem::CanMove G2048Problem::canMove(const G2048State& state) const
{
    G2048Problem::CanMove retval{};
    for (size_t x = 0; x < BOARD_DIMS; ++x)
    {
        for (size_t y = 0; y < BOARD_DIMS; ++y)
        {
            auto c = state.board(x, y);
            if (x < BOARD_DIMS - 1)
            {
                auto cright = state.board(x + 1, y);
                if (cright > 0 && (c == 0 || c == cright))
                {
                    retval.left = true;
                }
                if (c > 0 && (cright == 0 || c == cright))
                {
                    retval.right = true;
                }
            }
            if (y < BOARD_DIMS - 1)
            {
                auto cbelow = state.board(x, y + 1);
                if (cbelow > 0 && (c == 0 || c == cbelow))
                {
                    retval.up = true;
                }
                if (c > 0 && (cbelow == 0 || c == cbelow))
                {
                    retval.down = true;
                }
            }
            if (retval.all())
            {
                return retval;
            }
        }
    }
    return retval;
}

[[nodiscard]] size_t G2048Problem::countEmptyCells(const G2048State& state) const
{
    return state.board().numEmpty();
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
