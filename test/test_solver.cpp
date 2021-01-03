#include "mcts/problem.h"
#include "mcts/solver.h"
#include "mcts/state.h"
#include "mcts/tree_export.h"
#include "zbo/max_size_vector.h"

#include <gtest/gtest.h>

#include <cassert>
#include <optional>

enum class SelectCoin
{
    HEADS = 10,
    TAILS = 20
};

std::string to_string(SelectCoin c)
{
    switch (c)
    {
        case SelectCoin::HEADS:
            return "HEADS";
        case SelectCoin::TAILS:
            return "TAILS";
    }

    assert(false);
    return "";
}

std::ostream& operator<<(std::ostream& str, SelectCoin c)
{
    return str << to_string(c);
}

struct RiggedToinCossState : public mcts::State<RiggedToinCossState>
{
    std::optional<SelectCoin> player;
    std::optional<SelectCoin> world;
};

struct ProblemDefinition
{
    using ValueType = float;
    using ActionType = SelectCoin;
    using ChanceEventType = SelectCoin;
    using StateType = RiggedToinCossState;

    static constexpr int NUM_PLAYERS = 1;
    static constexpr int MAX_NUM_ACTIONS = 2;
    static constexpr int MAX_CHANCE_EVENTS = 2;

    using ValueVector = ValueType;
};

class RiggedToinCossProblem : public mcts::Problem<RiggedToinCossProblem, ProblemDefinition>
{
  public:
    [[nodiscard]] zbo::MaxSizeVector<ActionType, 2> getAvailableActions(const RiggedToinCossState& state) const
    {
        zbo::MaxSizeVector<ActionType, 2> retval{};

        if (!state.player.has_value())  // player did not perform any action yet
        {
            retval.push_back(SelectCoin::HEADS);
            retval.push_back(SelectCoin::TAILS);
        }
        return retval;
    }

    /**
     * Should return a list of all possible chance events with probability
     */
    [[nodiscard]] auto getAvailableChanceEvents(const RiggedToinCossState& state) const
    {
        zbo::MaxSizeVector<std::pair<float, SelectCoin>, 2> retval;
        if (state.player.has_value() && !state.world.has_value())
        {
            retval.push_back({PROBABILITY_HEADS, SelectCoin::HEADS});
            retval.push_back({1.0 - PROBABILITY_HEADS, SelectCoin::TAILS});
        }
        return retval;
    }

    [[nodiscard]] static mcts::StageType getNextStageType(const RiggedToinCossState& state)
    {
        if (state.player.has_value() && !state.world.has_value()) { return mcts::StageType::CHANCE; }
        else
        {
            return mcts::StageType::DECISION;
        }
    }

    [[nodiscard]] bool isTerminal(const RiggedToinCossState& state) const
    {
        return state.player.has_value() && state.world.has_value();
    };

    ValueVector performAction(const ActionType action, RiggedToinCossState& state) const
    {
        state.player = action;
        return {};
    }

    ValueVector performChanceEvent(const ChanceEventType& ce, RiggedToinCossState& state) const
    {
        state.world = ce;

        if (state.player == state.world) { return 1; }
        return {};
    }

    ValueVector performRandomChanceEvent(RiggedToinCossState& state) const
    {
        if (bernoulli(engine)) { return performChanceEvent(SelectCoin::HEADS, state); }
        return performChanceEvent(SelectCoin::TAILS, state);
    }

    ValueVector performRandomAction(RiggedToinCossState& state) const
    {
        auto actions = getAvailableActions(state);
        auto ac = actions[engine() % actions.size()];
        return performAction(ac, state);
    }

    static constexpr double PROBABILITY_HEADS = 0.7;

    mutable std::minstd_rand0 engine{};
    mutable std::bernoulli_distribution bernoulli{PROBABILITY_HEADS};
};

TEST(Problem, Constants)
{
    RiggedToinCossProblem problem{};

    ASSERT_EQ(problem.MAX_CHANCE_EVENTS, 2);
    ASSERT_EQ(problem.MAX_NUM_ACTIONS, 2);
}

TEST(Problem, Flow)
{
    RiggedToinCossState state{};
    RiggedToinCossProblem problem{};

    ASSERT_FALSE(problem.isTerminal(state));
    ASSERT_EQ(problem.getNextStageType(state), mcts::StageType::DECISION);
    auto actions = problem.getAvailableActions(state);
    ASSERT_EQ(actions.size(), 2);

    auto value = problem.performAction(SelectCoin::HEADS, state);
    ASSERT_FLOAT_EQ(value, 0.0f);

    ASSERT_FALSE(problem.isTerminal(state));
    ASSERT_EQ(problem.getNextStageType(state), mcts::StageType::CHANCE);
    auto events = problem.getAvailableChanceEvents(state);
    ASSERT_EQ(events.size(), 2);

    value = problem.performChanceEvent(SelectCoin::HEADS, state);
    ASSERT_FLOAT_EQ(value, 1.0f);
    ASSERT_TRUE(problem.isTerminal(state));
}

TEST(Solver, GT)
{
    RiggedToinCossState state{};
    RiggedToinCossProblem problem{};

    mcts::Solver<RiggedToinCossProblem> solver{};

    constexpr size_t START_ITERATIONS = 10;
    constexpr size_t NUM_INCREASE_ITERATIONS = 5;

    solver.parameter().numIterations = START_ITERATIONS;

    for (size_t i = 0; i < NUM_INCREASE_ITERATIONS; i++)
    {
        std::cout << "iterations " << solver.parameter().numIterations << "\n";
        auto action = solver.run(problem, state);
        solver.printTopLevelUtilities();
        EXPECT_EQ(action, SelectCoin::HEADS);
        solver.parameter().numIterations *= START_ITERATIONS;

        mcts::dot::exportTreeToDot<RiggedToinCossProblem>(solver.tree(), std::cout);
        mcts::dot::exportTreeToDot<RiggedToinCossProblem>(solver.tree(), "tree.dot");
    }
}