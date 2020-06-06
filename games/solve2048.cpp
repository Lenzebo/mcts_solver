
#include <iostream>
#include "../mcts/solver.h"
#include "../mcts/selection/ucb1.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "2048.h"
#include "../mcts/solver.h"

using namespace mcts;

float rolloutOnce(g2048::G2048State state, const g2048::G2048Problem& game, size_t desDepth = 100)
{
    float retval = 0.0f;
    size_t depth = 0;
    constexpr float discount = 0.99;
    float currDiscount = 1;
    while (!game.isTerminal(state))
    {
        switch (game.getNextStageType(state))
        {
            case mcts::StageType::DECISION:
            {
                auto reward = game.performRandomAction(state, state);
                retval = retval + currDiscount * reward[0];
                currDiscount *= discount;
                break;
            }
            case mcts::StageType::CHANCE:
            {
                auto reward = game.performRandomChanceEvent(state);
                retval = retval + reward[0];
                break;
            }
        }

        depth++;
        if (depth > desDepth)
        {
            break;
        }
    }
    return retval;
}

float rolloutMultiple(g2048::G2048State state, const g2048::G2048Problem& game, size_t desDepth = 100, size_t N = 100)
{
    float avg = 0;
    for (size_t i = 0; i < N; ++i)
    {
        avg += rolloutOnce(state, game, desDepth);
    }
    return avg / N;
}

g2048::Actions getBestAction(g2048::G2048State state, const g2048::G2048Problem& game)
{
    float bestValue = 0;
    ActionId bestAction{};
    for (uint8_t i = 0; i < 4; ++i)
    {
        ActionId action{i};
        auto ac = game.idToAction(state, action);
        g2048::G2048State cs = state;
        game.performAction(ac, cs, cs);
        if (!cs.isChanceNext())
        {
            // action didn't do anything, so we skip it
            continue;
        }
        float value = rolloutMultiple(cs, game);
        //        std::cout << "Action " << game.actionToString(state, ac) << " has a mean value of " << value << "\n";
        if (value > bestValue)
        {
            bestAction = action;
            bestValue = value;
        }
    }
    return game.idToAction(state, bestAction);
}

struct Result
{
    float score;
    size_t biggestTile;
};

template <typename Solver>
Result playG2048(Solver& solver)
{
    g2048::G2048State state;
    g2048::G2048Problem game(std::random_device().operator()());
    g2048::G2048Problem::ValueVector rewards{};
    while (!game.isTerminal(state))
    {
        if (state.getCurrentPlayer() == 0 && game.getNextStageType(state) == mcts::StageType::DECISION)
        {
            auto action = solver.run(game, state);
            //            auto action = getBestAction(state, game);
            state.print();
            solver.printTopLevelUtilities();
            rewards[0] += game.performAction(action, state, state)[0];

            //            std::cout << "--> Action: " << game.actionToString(state, action);
        }
        else
        {
            game.performRandomChanceEvent(state);
        }
        //        state.print();
    }
    state.print();
    std::cout << "Final score is " << rewards[0] << "\n";

    return {rewards[0], state.biggestTile()};
}

Result playG2048(size_t numIterations = 1000)
{
    mcts::UCB1SelectionPolicy<float> policy(UCB1SelectionPolicy<float>::Parameter{0, 100, 5});
    mcts::Solver<g2048::G2048Problem> solver(std::move(policy));
    solver.parameter().numIterations = numIterations;

    return playG2048(solver);
}

class G2048RolloutPolicy
{
  public:
    g2048::Actions getAction(const MaxSizeVector<g2048::Actions, 4>& availableActions, size_t id) const
    {
        std::array<g2048::Actions, 4> ordering = {g2048::Actions::RIGHT, g2048::Actions::DOWN, g2048::Actions::UP,
                                                  g2048::Actions::LEFT};
        if (id % 2 == 0)  // try to alternate between first two actions
        {
            std::swap(ordering[0], ordering[1]);
        }

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

    g2048::G2048Problem::ValueVector rollout(g2048::G2048State state, const g2048::G2048Problem& problem) const
    {
        g2048::G2048Problem::ValueVector retval{};
        size_t step = 0;
        while (!problem.isTerminal(state))
        {
            switch (problem.getNextStageType(state))
            {
                case mcts::StageType::DECISION:
                {
                    auto actions = problem.getAvailableActions(state);
                    auto action = getAction(actions, step);
                    step++;
                    auto reward = problem.performAction(action, state, state);
                    retval = retval + reward;
                    break;
                }
                case mcts::StageType::CHANCE:
                {
                    auto reward = problem.performRandomChanceEvent(state);
                    retval = retval + reward;
                    break;
                }
            }
        }
        return retval;
    }
};

Result playG2048HeuristicRollout(size_t numIterations = 1000)
{
    mcts::UCB1SelectionPolicy<float> policy(UCB1SelectionPolicy<float>::Parameter{0, 100, 5});

    mcts::Solver<g2048::G2048Problem, UCB1SelectionPolicy<float>, G2048RolloutPolicy> solver(std::move(policy), {});
    solver.parameter().numIterations = numIterations;

    return playG2048(solver);
}

int main(int, char**)
{
    std::vector<Result> results;
    for (size_t i = 0; i < 10; ++i)
    {
        results.push_back(playG2048HeuristicRollout(5000));
    }

    for (const auto& result : results)
    {
        std::cout << "Score: " << result.score << ", biggest tile: " << result.biggestTile << "\n";
    }
}