#pragma once

#include <assert.h>
#include <array>
#include <iostream>

#include "mcts/state.h"
#include "mcts/problem.h"
#include "mcts/utils/max_size_vector.h"

#define BOARD_EMPTY 0

namespace ttt {
class TicTacToeState : public mcts::State<TicTacToeState>
{
  public:
    TicTacToeState() { resetBoard(); }
    void resetBoard()
    {
        num_remaining_actions = 9;
        board.fill(BOARD_EMPTY);
    }

    std::ostream& writeToStream(std::ostream& stream) const
    {
        for (uint8_t i = 0; i < 9; ++i)
        {
            stream << int(board[i]) << " ";
            if (i % 3 == 2)
            {
                stream << "\n";
            }
        }
        return mcts::State<TicTacToeState>::writeToStream(stream);
    }

    std::array<uint8_t, 9> board;
    uint8_t num_remaining_actions;
};

enum Actions
{
    TOP_LEFT = 0,
    TOP_MIDDLE = 1,
    TOP_RIGHT = 2,
    MIDDLE_LEFT = 3,
    MIDDLE = 4,
    MIDDLE_RIGHT = 5,
    BOTTOM_LEFT = 6,
    BOTTOM_MIDDLE = 7,
    BOTTOM_RIGHT = 8
};

struct ProblemDefinition
{
    using ValueType = float;
    using ActionType = Actions;
    using ChanceEventType = mcts::NoEvent;
    using StateType = TicTacToeState;

    static constexpr int numPlayers = 2;
    static constexpr int maxNumActions = 9;
    static constexpr int maxChanceEvents = 0;

    using ValueVector = std::array<float, numPlayers>;
};

class TicTacToeProblem : public mcts::Problem<TicTacToeProblem, ProblemDefinition>
{
  public:
    TicTacToeProblem(){};

    mcts::StageType getNextStageType(const TicTacToeState&) const { return mcts::StageType::DECISION; }

    /**
     * Should return a list of all possible actions for this player
     */
    [[nodiscard]] mcts::MaxSizeVector<ActionType, 9> getAvailableActions(const TicTacToeState& state) const
    {
        mcts::MaxSizeVector<ActionType, 9> remaining_actions;
        if (state.num_remaining_actions == 0)  // if somebody did win, then there are no possible actions anymore
        {
            return {};
        }
        else if (state.num_remaining_actions ==
                 9)  // for the first move, we can apply symmetric reduction of the state space
        {
            remaining_actions.reserve(3);
            remaining_actions.push_back(TOP_LEFT);    // corner
            remaining_actions.push_back(TOP_MIDDLE);  // side
            remaining_actions.push_back(MIDDLE);      // middle
        }
        else if (state.num_remaining_actions == 8 &&
                 (state.board[TOP_LEFT] != BOARD_EMPTY || state.board[TOP_MIDDLE] != BOARD_EMPTY ||
                  state.board[MIDDLE] !=
                      BOARD_EMPTY))  // second move, we can apply symmetric reduction of the state space
        {
            if (state.board[MIDDLE] != BOARD_EMPTY)  // first move was middle
            {
                remaining_actions.reserve(2);
                remaining_actions.push_back(TOP_LEFT);    // corner
                remaining_actions.push_back(TOP_MIDDLE);  // side
            }
            else if (state.board[TOP_LEFT] != BOARD_EMPTY)  // first move was left top corner
            {
                remaining_actions.reserve(2);

                remaining_actions.push_back(TOP_MIDDLE);    // side
                remaining_actions.push_back(TOP_RIGHT);     // right top corner
                remaining_actions.push_back(MIDDLE);        // middle
                remaining_actions.push_back(MIDDLE_RIGHT);  // right
                remaining_actions.push_back(BOTTOM_RIGHT);  // bottom right
            }
            else if (state.board[1] != BOARD_EMPTY)  // first move was top
            {
                remaining_actions.reserve(2);

                remaining_actions.push_back(TOP_LEFT);
                remaining_actions.push_back(MIDDLE_LEFT);
                remaining_actions.push_back(BOTTOM_LEFT);
                remaining_actions.push_back(MIDDLE);
                remaining_actions.push_back(BOTTOM_MIDDLE);
            }
        }
        else
        {
            remaining_actions.reserve(state.num_remaining_actions);

            for (uint8_t i = 0; i < 9; ++i)
            {
                if (state.board[i] == BOARD_EMPTY)
                {
                    remaining_actions.push_back(static_cast<Actions>(i));
                }
                else
                {
                }
            }
        }
        return remaining_actions;
    };

    /**
     * Apply the action to the gamestate. Gamestate will be changed with this
     */
    ValueVector performAction(const ActionType action, TicTacToeState& state) const
    {
        assert(action < 9 && "Action identifier must be between 0 and 8");
        assert(state.board[action] == BOARD_EMPTY && "Action is not possible, because already played");
        ValueVector retval{};

        state.board[action] = state.getCurrentPlayer() + 1;
        state.num_remaining_actions--;

        assert(state.num_remaining_actions <= 9);

        if (didPlayerWin(state, state.getCurrentPlayer()))
        {
            retval[state.getCurrentPlayer()] = 1;
            state.num_remaining_actions = 0;
        }
        else if (state.num_remaining_actions == 0)
        {
            retval[0] = 0.5;
            retval[1] = 0.5;
        }
        state.increasePlayer(2);
        return retval;
    }

    /**
     * Determines whether the game is over. That can be if no actions are possible or if another desired or undesired
     * state is acchieved
     */
    [[nodiscard]] bool isTerminal(const TicTacToeState& state) const { return (state.num_remaining_actions == 0); };

    [[nodiscard]] bool didPlayerWin(const TicTacToeState& state, uint8_t player) const
    {
        if (state.num_remaining_actions > 4)
        {
            return false;
        }

        uint8_t pp1 = player + 1;
        return (((state.board[0] == pp1) && (state.board[1] == pp1) && (state.board[2] == pp1)) ||
                ((state.board[3] == pp1) && (state.board[4] == pp1) && (state.board[5] == pp1)) ||
                ((state.board[6] == pp1) && (state.board[7] == pp1) && (state.board[8] == pp1)) ||
                ((state.board[0] == pp1) && (state.board[3] == pp1) && (state.board[6] == pp1)) ||
                ((state.board[1] == pp1) && (state.board[4] == pp1) && (state.board[7] == pp1)) ||
                ((state.board[2] == pp1) && (state.board[5] == pp1) && (state.board[8] == pp1)) ||
                ((state.board[0] == pp1) && (state.board[4] == pp1) && (state.board[8] == pp1)) ||
                ((state.board[2] == pp1) && (state.board[4] == pp1) && (state.board[6] == pp1)));
    }
};

class TicTacToePolicy
{
  public:
    static Actions getAction(const TicTacToeState& state, const TicTacToeProblem& )
    {
        assert(state.num_remaining_actions > 0);

        // check whether we have to stop a instant win, or if we can win instantly
        for (uint8_t i = 0; i < 9; ++i)
        {
            uint8_t row = i / 3;
            uint8_t column = i % 3;
            Actions action = static_cast<Actions>(i);
            if (state.board[i] == BOARD_EMPTY)
            {
                // check rows:
                if (state.board[row * 3 + (column + 1) % 3] == state.board[row * 3 + (column + 2) % 3] &&
                    state.board[row * 3 + (column + 2) % 3] != BOARD_EMPTY)
                {
                    return action;
                }
                else if (state.board[((row + 1) % 3) * 3 + column] == state.board[((row + 2) % 3) * 3 + column] &&
                         state.board[((row + 1) % 3) * 3 + column] != BOARD_EMPTY)
                {
                    return action;
                }
                else if (row == column &&
                         state.board[((row + 1) % 3) * 3 + (column + 1) % 3] ==
                         state.board[((row + 2) % 3) * 3 + (column + 2) % 3] &&
                         state.board[((row + 2) % 3) * 3 + (column + 2) % 3] != BOARD_EMPTY)
                {
                    return action;
                }
                else if (row == 2 - column &&
                         state.board[((row + 1) % 3) * 3 + (column + 3 - 1) % 3] ==
                         state.board[((row + 2) % 3) * 3 + (column + 3 - 2) % 3] &&
                         state.board[((row + 2) % 3) * 3 + (column + 3 - 2) % 3] != BOARD_EMPTY)
                {
                    return action;
                }
            }
        }

        uint8_t actidx = rand() % state.num_remaining_actions;
        for (uint8_t i = 0; i < 9; ++i)
        {
            if (state.board[i] == BOARD_EMPTY)
            {
                if (actidx == 0)
                {
                    return static_cast<Actions>(i);
                }
                else
                {
                    actidx--;
                }
            }
        }
        assert(false);
        return {};
    }
};
}