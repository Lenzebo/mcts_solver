#include "mcts/solver.h"
#include "tic_tac_toe.h"
#include "zbo/stop_watch.h"

#include <chrono>
#include <iostream>

using namespace mcts;
using namespace ttt;

int testMCTSTicTacToe(bool switchPlayers, size_t& mctsIterations)
{
    TicTacToeState state;
    TicTacToeProblem game;
    TicTacToeProblem::ValueVector rewards;
    static mcts::Solver<TicTacToeProblem, UCB1SelectionPolicy<float>, RolloutPolicy<ttt::TicTacToePolicy> > solver;
    solver.parameter().numIterations = 1000;  // NOLINT

    while (!game.isTerminal(state))
    {
        if ((state.getCurrentPlayer() == 0 && !switchPlayers) || (state.getCurrentPlayer() == 1 && switchPlayers))
        {
            auto action = solver.run(game, state);
            rewards = game.performAction(action, state);
            mctsIterations += solver.parameter().numIterations;
        }
        else
        {
            auto action = solver.run(game, state);
            rewards = game.performAction(action, state);
            mctsIterations += solver.parameter().numIterations;
        }
    }

    int result = int(rewards[0] * 2 - 1);
    if (result == 1)
    {
        return 1 - switchPlayers;
    }
    else if (result == -1)
    {
        return switchPlayers;
    }
    else
    {
        return -1;
    }
}

int main(int, char**)
{
    constexpr uint32_t NUM_ITERATIONS = 1000;

    uint32_t numWins1 = 0;
    uint32_t numWins2 = 0;
    uint32_t numDraws = 0;

    size_t mctsIterations = 0;

    zbo::StopWatch total;
    total.start();
    for (size_t i = 0; i < NUM_ITERATIONS; ++i)
    {
        zbo::StopWatch iteration;
        iteration.start();

        const bool switchPlayers = i % 2 == 0;
        const int retval = testMCTSTicTacToe(switchPlayers, mctsIterations);
        if (retval == 0)
        {
            numWins1++;
        }
        else if (retval == 1)
        {
            numWins2++;
        }
        else
        {
            numDraws++;
        }
        auto dt = iteration.stop();

        // print formatted date
        std::cout << "Game # " << i << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(dt).count()
                  << "ms" << std::endl;
    }

    auto dttotal = total.stop();

    // print formatted date
    std::cout << "All games took " << std::chrono::duration_cast<std::chrono::milliseconds>(dttotal).count()
              << "ms, which is a mean duration of "
              << double(std::chrono::duration_cast<std::chrono::milliseconds>(dttotal).count()) / NUM_ITERATIONS
              << "ms. and a duration of "
              << double(std::chrono::duration_cast<std::chrono::microseconds>(dttotal).count()) / mctsIterations
              << "us per mcts iteration" << std::endl;

    constexpr int PERCENT = 100;
    std::cout << "Player 1: " << double(numWins1) / double(NUM_ITERATIONS) * PERCENT << "%\n";
    std::cout << "Player 2: " << double(numWins2) / double(NUM_ITERATIONS) * PERCENT << "%\n";
    std::cout << "Draws:    " << double(numDraws) / double(NUM_ITERATIONS) * PERCENT << "%\n";
}