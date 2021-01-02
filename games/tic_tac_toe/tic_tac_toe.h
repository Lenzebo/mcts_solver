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

#include "mcts/problem.h"
#include "mcts/state.h"
#include "zbo/max_size_vector.h"
#include "zbo/meta_enum.h"

#include <array>
#include <cassert>
#include <iostream>

namespace ttt {

constexpr size_t BOARD_SIZE = 9;

constexpr std::array<std::array<size_t, 3>, 8> WIN_PATTERNS{{
    {0, 1, 2},  // row 1
    {3, 4, 5},  // row 2
    {6, 7, 8},  // row 3
    {0, 3, 6},  // col 1
    {1, 4, 7},  // col 2
    {2, 5, 8},  // col 3
    {0, 4, 8},  // diag 1
    {2, 4, 6}   // diag 2
}};

ZBO_ENUM_CLASS(FieldType, uint8_t, EMPTY = 0, PLAYER1 = 1, PLAYER2 = 2)
ZBO_ENUM_CLASS(Actions, uint8_t, TOP_LEFT = 0, TOP_MIDDLE = 1, TOP_RIGHT = 2, MIDDLE_LEFT = 3, MIDDLE = 4,
               MIDDLE_RIGHT = 5, BOTTOM_LEFT = 6, BOTTOM_MIDDLE = 7, BOTTOM_RIGHT = 8)

inline std::string to_string(Actions action)  // NOLINT
{
    return std::string(zbo::enumToString(action));
}

inline std::ostream& operator<<(std::ostream& stream, Actions action)
{
    return stream << zbo::enumToString(action);
}

constexpr float WIN = 1.0f;
class TicTacToeState : public mcts::State<TicTacToeState>
{
  public:
    TicTacToeState() { resetBoard(); }
    void resetBoard()
    {
        numRemainingActions = BOARD_SIZE;
        board.fill(FieldType::EMPTY);
    }

    std::ostream& writeToStream(std::ostream& stream) const
    {
        for (uint8_t i = 0; i < BOARD_SIZE; ++i)
        {
            stream << int(board.at(i)) << " ";
            if (i % 3 == 2)
            {
                stream << "\n";
            }
        }
        return mcts::State<TicTacToeState>::writeToStream(stream);
    }

    std::array<FieldType, BOARD_SIZE> board{};
    uint8_t numRemainingActions{BOARD_SIZE};
};

constexpr size_t actionToBoardIdx(Actions action)
{
    return static_cast<size_t>(action);
}

struct ProblemDefinition
{
    using ValueType = float;
    using ActionType = Actions;
    using ChanceEventType = mcts::NoEvent;
    using StateType = TicTacToeState;

    static constexpr int NUM_PLAYERS = 2;
    static constexpr int MAX_NUM_ACTIONS = 9;
    static constexpr int MAX_CHANCE_EVENTS = 0;

    using ValueVector = std::array<float, NUM_PLAYERS>;
};

class TicTacToeProblem : public mcts::Problem<TicTacToeProblem, ProblemDefinition>
{
  public:
    TicTacToeProblem() = default;

    [[nodiscard]] constexpr mcts::StageType getNextStageType(const TicTacToeState&) const
    {
        return mcts::StageType::DECISION;
    }

    /**
     * Should return a list of all possible actions for this player
     */
    [[nodiscard]] zbo::MaxSizeVector<Actions, BOARD_SIZE> getAvailableActions(const TicTacToeState& state) const
    {
        using ActionVec = zbo::MaxSizeVector<Actions, BOARD_SIZE>;

        if (state.numRemainingActions == 0)  // if somebody did win, then there are no possible actions anymore
        {
            return {};
        }
        else if (state.numRemainingActions ==
                 BOARD_SIZE)  // for the first move, we can apply symmetric reduction of the state space
        {
            return ActionVec{Actions::TOP_LEFT, Actions::TOP_MIDDLE, Actions::MIDDLE};
        }
        else if (state.numRemainingActions == BOARD_SIZE - 1 &&
                 (state.board[actionToBoardIdx(Actions::TOP_LEFT)] != FieldType::EMPTY ||
                  state.board[actionToBoardIdx(Actions::TOP_MIDDLE)] != FieldType::EMPTY ||
                  state.board[actionToBoardIdx(Actions::MIDDLE)] !=
                      FieldType::EMPTY))  // second move, we can apply symmetric reduction of the state space
        {
            if (state.board[actionToBoardIdx(Actions::MIDDLE)] != FieldType::EMPTY)  // first move was middle
            {
                return ActionVec{Actions::TOP_LEFT, Actions::TOP_MIDDLE};
            }
            else if (state.board[actionToBoardIdx(Actions::TOP_LEFT)] !=
                     FieldType::EMPTY)  // first move was left top corner
            {
                return ActionVec{Actions::TOP_MIDDLE, Actions::TOP_RIGHT, Actions::MIDDLE, Actions::MIDDLE_RIGHT,
                                 Actions::BOTTOM_RIGHT};
            }
            else if (state.board[1] != FieldType::EMPTY)  // first move was top
            {
                return ActionVec{Actions::TOP_LEFT, Actions::MIDDLE_LEFT, Actions::BOTTOM_LEFT, Actions::MIDDLE,
                                 Actions::BOTTOM_MIDDLE};
            }
        }

        ActionVec remaining{};
        for (uint8_t i = 0; i < BOARD_SIZE; ++i)
        {
            if (state.board.at(i) == FieldType::EMPTY)
            {
                remaining.push_back(static_cast<Actions>(i));
            }
        }
        return remaining;
    };

    /**
     * Apply the action to the gamestate. Gamestate will be changed with this
     */
    ValueVector performAction(const Actions action, TicTacToeState& state) const
    {
        assert(state.board.at(actionToBoardIdx(action)) == FieldType::EMPTY &&
               "Action is not possible, because already played");
        ValueVector retval{};

        state.board.at(actionToBoardIdx(action)) = static_cast<FieldType>(state.getCurrentPlayer() + 1);
        state.numRemainingActions--;

        assert(state.numRemainingActions <= 9);

        if (didPlayerWin(state, state.getCurrentPlayer()))
        {
            retval[state.getCurrentPlayer()] = WIN;
            state.numRemainingActions = 0;
        }
        else if (state.numRemainingActions == 0)
        {
            retval[0] = WIN / 2;
            retval[1] = WIN / 2;
        }
        state.increasePlayer(2);
        return retval;
    }

    /**
     * Determines whether the game is over. That can be if no actions are possible or if another desired or undesired
     * state is acchieved
     */
    [[nodiscard]] bool isTerminal(const TicTacToeState& state) const { return (state.numRemainingActions == 0); };

    [[nodiscard]] bool didPlayerWin(const TicTacToeState& state, uint8_t player) const
    {
        if (state.numRemainingActions > 4)
        {
            return false;
        }

        const auto pp1 = static_cast<FieldType>(player + 1);

        return std::any_of(WIN_PATTERNS.begin(), WIN_PATTERNS.end(),
                           [&state, pp1](const std::array<size_t, 3>& pattern) {
                               return std::all_of(pattern.begin(), pattern.end(),
                                                  [&state, pp1](size_t idx) { return state.board.at(idx) == pp1; });
                           });
    }
};

class TicTacToePolicy
{
  public:
    static Actions getAction(const TicTacToeState& state, const TicTacToeProblem&)
    {
        assert(state.numRemainingActions > 0);

        // check whether we have to stop a instant win, or if we can win instantly
        for (uint8_t i = 0; i < BOARD_SIZE; ++i)
        {
            uint8_t row = i / 3;
            uint8_t column = i % 3;
            auto action = static_cast<Actions>(i);
            if (state.board.at(i) == FieldType::EMPTY)
            {
                // check rows:
                if (state.board.at(row * 3 + (column + 1) % 3) == state.board.at(row * 3 + (column + 2) % 3) &&
                    state.board.at(row * 3 + (column + 2) % 3) != FieldType::EMPTY)
                {
                    return action;
                }
                else if (state.board.at(((row + 1) % 3) * 3 + column) == state.board.at(((row + 2) % 3) * 3 + column) &&
                         state.board.at(((row + 1) % 3) * 3 + column) != FieldType::EMPTY)
                {
                    return action;
                }
                else if (row == column &&
                         state.board.at(((row + 1) % 3) * 3 + (column + 1) % 3) ==
                             state.board.at(((row + 2) % 3) * 3 + (column + 2) % 3) &&
                         state.board.at(((row + 2) % 3) * 3 + (column + 2) % 3) != FieldType::EMPTY)
                {
                    return action;
                }
                else if (row == 2 - column &&
                         state.board.at(((row + 1) % 3) * 3 + (column + 3 - 1) % 3) ==
                             state.board.at(((row + 2) % 3) * 3 + (column + 3 - 2) % 3) &&
                         state.board.at(((row + 2) % 3) * 3 + (column + 3 - 2) % 3) != FieldType::EMPTY)
                {
                    return action;
                }
            }
        }

        uint8_t actidx = rand() % state.numRemainingActions;
        for (uint8_t i = 0; i < BOARD_SIZE; ++i)
        {
            if (state.board.at(i) == FieldType::EMPTY)
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
}  // namespace ttt