
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

constexpr size_t DEFAULT_NUM_ITERATIONS = 1000;

struct Result
{
    float score{};
    size_t biggestTile{};
    g2048::G2048State finalState;
    size_t numMoves{};
    std::chrono::microseconds duration{};
};

template <typename Policy>
Result playG2048WithPolicy(Policy& policy)
{
    g2048::G2048State state;
    g2048::G2048Problem game(std::random_device().operator()());
    g2048::G2048Problem::ValueVector rewards{};
    size_t numMoves = 0;
    const auto start = std::chrono::system_clock::now();
    while (!game.isTerminal(state))
    {
        if (game.getNextStageType(state) == mcts::StageType::DECISION)
        {
            auto action = policy.getAction(state, game);
            rewards += game.performAction(action, state);
            numMoves++;
        }
        else
        {
            rewards += game.performRandomChanceEvent(state);
        }
    }

    const auto end = std::chrono::system_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return {rewards, state.board().biggestTile(), state, numMoves, duration};
}

auto getMCTSSolverRandomRollout(size_t numIterations = DEFAULT_NUM_ITERATIONS)
{
    mcts::UCB1SelectionPolicy<float> selectionPolicy({0, 500, 5});  // NOLINT
    mcts::Solver<g2048::G2048Problem> solver(std::move(selectionPolicy));
    solver.parameter().numIterations = numIterations;
    return solver;
}

auto getMCTSSolverHeuristicRollout(size_t numIterations = DEFAULT_NUM_ITERATIONS)
{
    using G2048RolloutPolicy = mcts::RolloutPolicy<g2048::FixedSequencePolicy>;

    mcts::UCB1SelectionPolicy<float> selectionPolicy(UCB1SelectionPolicy<float>::Parameter{0, 500, 5});  // NOLINT
    mcts::Solver<g2048::G2048Problem, UCB1SelectionPolicy<float>, G2048RolloutPolicy> solver(
        std::move(selectionPolicy), G2048RolloutPolicy{100});  // NOLINT
    solver.parameter().numIterations = numIterations;

    return solver;
}

auto getMCTSSolverSmartRollout(size_t numIterations = DEFAULT_NUM_ITERATIONS)
{
    using G2048RolloutPolicy = mcts::RolloutPolicy<g2048::MCRolloutPolicy>;

    mcts::UCB1SelectionPolicy<float> selectionPolicy(UCB1SelectionPolicy<float>::Parameter{0, 500, 5});  // NOLINT
    mcts::Solver<g2048::G2048Problem, UCB1SelectionPolicy<float>, G2048RolloutPolicy> solver(
        std::move(selectionPolicy), {g2048::MCRolloutPolicy{1, 10, 0.95f}});  // NOLINT
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
        constexpr size_t PRINT_WIDTH = 5;
        std::cout << "Received tile " << std::setw(PRINT_WIDTH) << counter.first << " " << counter.second << " out of "
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
    constexpr size_t COUNT = 1000;

    {
        std::cout << "#### RandomPolicy: " << std::endl;
        evaluatePolicy(RandomPolicy{}, COUNT);
    }

    {
        std::cout << "#### BestPositionPolicy: " << std::endl;
        evaluatePolicy(g2048::BestPositionPolicy{}, COUNT);
    }

    {
        std::cout << "#### FixedSequence: " << std::endl;
        evaluatePolicy(g2048::FixedSequencePolicy{}, COUNT);
    }

    {
        std::cout << "#### MCRolloutPolicy (1 / 10 / 0.95): " << std::endl;
        evaluatePolicy(g2048::MCRolloutPolicy{10, 10, 0.95f}, COUNT);  // NOLINT
    }

    return 0;

    {
        std::cout << "#### MCRolloutPolicy: " << std::endl;
        evaluatePolicy(g2048::MCRolloutPolicy{}, COUNT);
    }

    {
        std::cout << "#### MCTS (100) with heuristic rollout: " << std::endl;
        auto solver = getMCTSSolverHeuristicRollout(100);  // NOLINT
        evaluatePolicy(g2048::MCTSPolicy{solver}, COUNT);
    }
    {
        std::cout << "#### MCTS (1000) with heuristic rollout: " << std::endl;
        auto solver = getMCTSSolverHeuristicRollout(500);  // NOLINT
        evaluatePolicy(g2048::MCTSPolicy{solver}, COUNT);
    }

    {
        std::cout << "#### MCTS (100) with random rollout: " << std::endl;
        auto solver = getMCTSSolverRandomRollout(100);  // NOLINT
        evaluatePolicy(g2048::MCTSPolicy{solver}, COUNT);
    }
    {
        std::cout << "#### MCTS (1000) with random rollout: " << std::endl;
        auto solver = getMCTSSolverRandomRollout(1000);  // NOLINT
        evaluatePolicy(g2048::MCTSPolicy{solver}, COUNT);
    }
}