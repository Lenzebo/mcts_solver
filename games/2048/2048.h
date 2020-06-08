#pragma once

#include <cassert>
#include <array>
#include <iostream>
#include <random>
#include <iomanip>

#include "mcts/state.h"
#include "mcts/problem.h"
#include "mcts/utils/max_size_vector.h"

#include "board.h"

namespace g2048 {

constexpr float PROBABILITY_SPAWN_2 = 0.8f;                        // probability for a 2 is 80%,
constexpr float PROBABILITY_SPAWN_4 = 1.0f - PROBABILITY_SPAWN_2;  // probability for a 4 is 20%

enum Actions
{
    UP = 0,
    LEFT = 1,
    RIGHT = 2,
    DOWN = 3
};

struct ChanceEvent
{
    uint8_t x;
    uint8_t y;
    uint8_t value;  // 2 or 4
};

class G2048Problem;

class G2048State : public mcts::State<G2048State>
{
  public:
    G2048State() : board_() { resetBoard(); }

    explicit G2048State(const Board& board, bool chance = false) : board_(board), nextIsChance(chance) {}
    void resetBoard() { board_ = {}; }

    std::ostream& writeToStream(std::ostream& stream) const;  // NOLINT

    [[nodiscard]] uint8_t board(size_t x, size_t y) const { return board_.at(x, y); }
    [[nodiscard]] bool empty(size_t x, size_t y) const;

    void setBoard(size_t x, size_t y, uint8_t value) { board_.set(x, y, value); }

    [[nodiscard]] const Board& board() const { return board_; }
    [[nodiscard]] bool isChanceNext() const { return nextIsChance; }
    void setNextChance(bool isChance) { nextIsChance = isChance; }

  private:
    Board board_{};
    bool nextIsChance = true;
};

struct ProblemDefinition
{
    using ValueType = float;
    using ActionType = Actions;
    using ChanceEventType = ChanceEvent;
    using StateType = G2048State;

    static constexpr int numPlayers = 1;
    static constexpr int maxNumActions = 4;
    static constexpr int maxChanceEvents = 32;

    using ValueVector = ValueType;
};

class G2048Problem : public mcts::Problem<G2048Problem, ProblemDefinition>
{
  public:
    G2048Problem() = default;
    G2048Problem(size_t seed) : engine(seed) {}
    std::string actionToString(const StateType& state, const ActionType& action) const;  // NOLINT

    static mcts::StageType getNextStageType(const G2048State& state);

    [[nodiscard]] mcts::MaxSizeVector<ActionType, 4> getAvailableActions(const G2048State& state) const;
    [[nodiscard]] mcts::MaxSizeVector<std::pair<float, ChanceEvent>, NUM_CELLS * 2> getAvailableChanceEvents(
        const G2048State& state) const;

    ValueVector performAction(const ActionType action, G2048State& state) const;
    ValueVector performChanceEvent(const ChanceEventType event, G2048State& state) const;

    ValueVector performRandomChanceEvent(G2048State& state) const;
    ProblemDefinition::ValueVector performRandomAction(G2048State& state) const;

    [[nodiscard]] bool isTerminal(const G2048State& state) const;

  private:
    struct CanMove
    {
        bool left{false};
        bool right{false};
        bool up{false};
        bool down{false};
        bool any() const { return left || right || up || down; }
        bool all() const { return left && right && up && down; }
    };

    CanMove canMove(const G2048State& state) const;
    [[nodiscard]] size_t countEmptyCells(const G2048State& state) const;

    void addRandomElement(G2048State& state) const;

  private:
    mutable std::minstd_rand0 engine{};
    mutable std::bernoulli_distribution bernoulli{PROBABILITY_SPAWN_2};
    mutable std::uniform_int_distribution<int> actionDist{0, 3};
};
}