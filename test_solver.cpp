#include "mcts/problem.h"
#include "mcts/state.h"
#include "mcts/utils/max_size_vector.h"
#include "mcts/solver.h"

#include <gtest/gtest.h>
#include <optional>

// template <int NUM_PLAYERS, int MAX_NUM_ACTIONS, typename STATE_TYPE, typename ACTION_TYPE = uint8_t,
//          typename VALUE_TYPE = float, int MAX_CHANCE_EVENTS = 0, typename CHANCE_TYPE = uint8_t>

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
}

struct RiggedToinCossState : public mcts::State<RiggedToinCossState>
{
    std::optional<SelectCoin> player;
    std::optional<SelectCoin> world;
};

class RiggedToinCossProblem : public mcts::Problem<1, 2, RiggedToinCossState, SelectCoin, float, 2, SelectCoin>
{
  public:
    [[nodiscard]] mcts::MaxSizeVector<ActionType, 2> getAvailableActions(const RiggedToinCossState& state) const
    {
        mcts::MaxSizeVector<ActionType, 2> retval{};

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
        mcts::MaxSizeVector<std::pair<float, SelectCoin>, 2> retval;
        if (state.player.has_value() && !state.world.has_value())
        {
            retval.push_back({0.7f, SelectCoin::HEADS});
            retval.push_back({0.3f, SelectCoin::TAILS});
        }
        return retval;
    }

    [[nodiscard]] static mcts::StageType getNextStageType(const RiggedToinCossState& state)
    {
        if (state.player.has_value() && !state.world.has_value())
        {
            return mcts::StageType::CHANCE;
        }
        else
        {
            return mcts::StageType::DECISION;
        }
    }

    [[nodiscard]] bool isTerminal(const RiggedToinCossState& state) const
    {
        return state.player.has_value() && state.world.has_value();
    };

    ValueVector performAction(const ActionType action, const RiggedToinCossState& before,
                              RiggedToinCossState& after) const
    {
        after = before;
        after.player = action;
        return {};
    }

    ValueVector performChanceEvent(const ChanceEventType& ce, const RiggedToinCossState& before,
                                   RiggedToinCossState& after) const
    {
        after = before;
        after.world = ce;

        if (after.player == after.world)
        {
            return {1};
        }
        return {};
    }

    ValueVector performRandomChanceEvent(RiggedToinCossState& state) const
    {
        if (bernoulli(engine))
        {
            return performChanceEvent(SelectCoin::HEADS, state, state);
        }
        return performChanceEvent(SelectCoin::TAILS, state, state);
    }

    ValueVector performRandomAction(const RiggedToinCossState& before, RiggedToinCossState& after) const
    {
        auto actions = getAvailableActions(before);
        auto ac = actions[engine() % actions.size()];
        return performAction(ac, before, after);
    }

    mutable std::minstd_rand0 engine{};
    mutable std::bernoulli_distribution bernoulli{0.7};
};

TEST(Problem, Constants)
{
    RiggedToinCossProblem problem{};

    ASSERT_EQ(problem.maxChanceEvents, 2);
    ASSERT_EQ(problem.maxNumActions, 2);
}

TEST(Problem, Flow)
{
    RiggedToinCossState state{};
    RiggedToinCossProblem problem{};

    ASSERT_FALSE(problem.isTerminal(state));
    ASSERT_EQ(problem.getNextStageType(state), mcts::StageType::DECISION);
    auto actions = problem.getAvailableActions(state);
    ASSERT_EQ(actions.size(), 2);

    auto value = problem.performAction(SelectCoin::HEADS, state, state);
    ASSERT_FLOAT_EQ(value[0], 0.0f);

    ASSERT_FALSE(problem.isTerminal(state));
    ASSERT_EQ(problem.getNextStageType(state), mcts::StageType::CHANCE);
    auto events = problem.getAvailableChanceEvents(state);
    ASSERT_EQ(events.size(), 2);

    value = problem.performChanceEvent(SelectCoin::HEADS, state, state);
    ASSERT_FLOAT_EQ(value[0], 1.0f);
    ASSERT_TRUE(problem.isTerminal(state));
}

TEST(Solver, GT)
{
    RiggedToinCossState state{};
    RiggedToinCossProblem problem{};

    mcts::Solver<RiggedToinCossProblem> solver{};

    solver.parameter().numIterations = 10;

    for (size_t i = 0; i < 10; i ++ )
    {
        std::cout << "iterations " << solver.parameter().numIterations << "\n";
        auto action = solver.run(problem, state);
        solver.printTopLevelUtilities();
        ASSERT_EQ(action, SelectCoin::HEADS);
        solver.parameter().numIterations *= 10;
    }

}