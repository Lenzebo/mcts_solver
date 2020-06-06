#pragma once

#include <cassert>
#include <array>
#include <iostream>
#include <random>
#include <iomanip>

#include "../mcts/state.h"
#include "../mcts/problem.h"
#include "mcts/utils/max_size_vector.h"

namespace g2048 {

constexpr size_t BOARD_DIMS = 4;
constexpr size_t NUM_CELLS = BOARD_DIMS * BOARD_DIMS;
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

struct Board
{
    uint8_t at(size_t x, size_t y) const { return values_[x][y]; }
    void set(size_t x, size_t y, uint8_t value) { values_[x][y] = value; }

  private:
    std::array<std::array<uint8_t, BOARD_DIMS>, BOARD_DIMS> values_{};
};

class G2048State : public mcts::State<G2048State>
{
  public:
    G2048State() : board_() { resetBoard(); }

    explicit G2048State(const Board& board, bool chance = false) : board_(board), nextIsChance(chance) {}
    void resetBoard() { board_ = {}; }

    std::ostream& writeToStream(std::ostream& stream) const;  // NOLINT

    [[nodiscard]] size_t biggestTile() const;
    [[nodiscard]] uint8_t board(size_t x, size_t y) const { return board_.at(x, y); }

    void setBoard(size_t x, size_t y, uint8_t value) { board_.set(x, y, value); }

    [[nodiscard]] Board board() const { return board_; }
    [[nodiscard]] bool isChanceNext() const { return nextIsChance; }
    void setNextChance(bool isChance) { nextIsChance = isChance; }

  private:
    Board board_{};
    bool nextIsChance = true;
};

class G2048Problem : public mcts::Problem<1, 4, G2048State, Actions, float, 32, ChanceEvent>
{
  public:
    G2048Problem() = default;
    G2048Problem(size_t seed) : engine(seed) {}
    std::string actionToString(const StateType& state, const ActionType& action) const;  // NOLINT

    static mcts::StageType getNextStageType(const G2048State& state);

    [[nodiscard]] mcts::MaxSizeVector<ActionType, 4> getAvailableActions(const G2048State& state) const;
    [[nodiscard]] mcts::MaxSizeVector<std::pair<float, ChanceEvent>, NUM_CELLS * 2> getAvailableChanceEvents(
        const G2048State& state) const;

    [[nodiscard]] ValueVector performAction(const ActionType action, const G2048State& before, G2048State& after) const;
    [[nodiscard]] ValueVector performChanceEvent(const ChanceEventType event, const G2048State& before,
                                                 G2048State& after) const;

    [[nodiscard]] ValueVector performRandomChanceEvent(G2048State& state) const;

    [[nodiscard]] bool isTerminal(const G2048State& state) const;

  private:
    bool canMoveLeft(const G2048State& state) const;
    bool canMoveRight(const G2048State& state) const;
    bool canMoveUp(const G2048State& state) const;
    bool canMoveDown(const G2048State& state) const;
    [[nodiscard]] size_t countEmptyCells(const G2048State& state) const;

    void addRandomElement(G2048State& state) const;

  private:
    mutable std::minstd_rand0 engine{};
    mutable std::bernoulli_distribution bernoulli{PROBABILITY_SPAWN_2};
    mutable std::uniform_int_distribution<int> actionDist{0, 3};
};
}