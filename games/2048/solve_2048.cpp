
#include "2048.h"
#include "mcts/selection/ucb1.h"
#include "mcts/solver.h"
#include "policies.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>

using namespace mcts;

struct Result
{
    float score;
    size_t biggestTile;
    g2048::G2048State finalState;
    size_t numMoves;
    std::chrono::microseconds duration;
};

template <typename Policy>
Result playG2048WithPolicy(Policy& policy)
{
    g2048::G2048State state;
    g2048::G2048Problem game(std::random_device().operator()());
    g2048::G2048Problem::ValueVector rewards{};
    size_t numMoves = 0;
    auto start = std::chrono::system_clock::now();
    while (!game.isTerminal(state))
    {
        if (game.getNextStageType(state) == mcts::StageType::DECISION)
        {
            auto action = policy.getAction(state, game);
            rewards += game.performAction(action, state);
            numMoves++;
            // state.print();
        }
        else
        {
            rewards += game.performRandomChanceEvent(state);
        }
    }
    // state.print();
    //    std::cout << "Final score is " << rewards << "\n";

    auto end = std::chrono::system_clock::now();
    return {rewards, state.board().biggestTile(), state, numMoves, end - start};
}

auto getMCTSSolverRandomRollout(size_t numIterations = 1000)
{
    mcts::UCB1SelectionPolicy<float> selectionPolicy({0, 500, 5});
    mcts::Solver<g2048::G2048Problem> solver(std::move(selectionPolicy));
    solver.parameter().numIterations = numIterations;
    return solver;
}

auto getMCTSSolverHeuristicRollout(size_t numIterations = 1000)
{
    using G2048RolloutPolicy = mcts::RolloutPolicy<g2048::FixedSequencePolicy>;

    mcts::UCB1SelectionPolicy<float> selectionPolicy(UCB1SelectionPolicy<float>::Parameter{0, 500, 5});
    mcts::Solver<g2048::G2048Problem, UCB1SelectionPolicy<float>, G2048RolloutPolicy> solver(std::move(selectionPolicy),
                                                                                             G2048RolloutPolicy{100});
    solver.parameter().numIterations = numIterations;

    return solver;
}

auto getMCTSSolverSmartRollout(size_t numIterations = 1000)
{
    using G2048RolloutPolicy = mcts::RolloutPolicy<g2048::MCRolloutPolicy>;

    mcts::UCB1SelectionPolicy<float> selectionPolicy(UCB1SelectionPolicy<float>::Parameter{0, 500, 5});
    mcts::Solver<g2048::G2048Problem, UCB1SelectionPolicy<float>, G2048RolloutPolicy> solver(
        std::move(selectionPolicy), {g2048::MCRolloutPolicy{1, 10, 0.95f}});
    solver.parameter().numIterations = numIterations;

    return solver;
}

void analyzeResults(std::vector<Result> results)
{
    std::sort(results.begin(), results.end(), [](const Result& r1, const Result& r2) { return r1.score < r2.score; });

    using namespace std::chrono;

    size_t overallMoves = 0;
    microseconds overallDuration{};
    std::map<size_t, size_t> tileCounter{};
    for (const auto& r : results)
    {
        tileCounter[r.biggestTile]++;
        overallMoves += r.numMoves;
        overallDuration += r.duration;
    }

    for (const auto& counter : tileCounter)
    {
        std::cout << "Received tile " << std::setw(5) << counter.first << " " << counter.second << " out of "
                  << results.size() << " times\n";
    }

    std::cout << "Evaluated " << overallMoves << " in " << duration_cast<milliseconds>(overallDuration).count()
              << " ms, averaged: " << double(overallDuration.count()) / overallMoves << "us/move \n";
}

template <typename Policy>
std::vector<Result> evaluatePolicy(Policy policy, size_t count)
{
    std::vector<Result> results;
    std::cout << "[";
    for (size_t i = 0; i < count; ++i)
    {
        results.push_back(playG2048WithPolicy(policy));
        //        std::cout << "##";
        std::cout.flush();
    }
    std::cout << "]\n";
    analyzeResults(results);
    return results;
}

int main(int, char**)
{
    size_t count = 1000;

    {
        std::cout << "#### RandomPolicy: " << std::endl;
        evaluatePolicy(RandomPolicy{}, count);
    }

    {
        std::cout << "#### BestPositionPolicy: " << std::endl;
        evaluatePolicy(g2048::BestPositionPolicy{}, count);
    }

    {
        std::cout << "#### FixedSequence: " << std::endl;
        evaluatePolicy(g2048::FixedSequencePolicy{}, count);
    }

    {
        std::cout << "#### MCRolloutPolicy (1 / 10 / 0.95): " << std::endl;
        evaluatePolicy(g2048::MCRolloutPolicy{10, 10, 0.95f}, count);
    }

    return 0;

    {
        std::cout << "#### MCRolloutPolicy: " << std::endl;
        evaluatePolicy(g2048::MCRolloutPolicy{}, count);
    }

    {
        std::cout << "#### MCTS (100) with heuristic rollout: " << std::endl;
        auto solver = getMCTSSolverHeuristicRollout(100);
        evaluatePolicy(g2048::MCTSPolicy{solver}, count);
    }
    {
        std::cout << "#### MCTS (1000) with heuristic rollout: " << std::endl;
        auto solver = getMCTSSolverHeuristicRollout(500);
        evaluatePolicy(g2048::MCTSPolicy{solver}, count);
    }

    {
        std::cout << "#### MCTS (100) with random rollout: " << std::endl;
        auto solver = getMCTSSolverRandomRollout(100);
        evaluatePolicy(g2048::MCTSPolicy{solver}, count);
    }
    {
        std::cout << "#### MCTS (1000) with random rollout: " << std::endl;
        auto solver = getMCTSSolverRandomRollout(1000);
        evaluatePolicy(g2048::MCTSPolicy{solver}, count);
    }
}