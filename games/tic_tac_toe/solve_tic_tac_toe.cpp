#include "mcts/solver.h"
#include "tic_tac_toe.h"

#include <chrono>
#include <iostream>

using namespace mcts;
using namespace ttt;

int testMCTSTicTacToe(bool switch_players, size_t& mctsIterations)
{
    TicTacToeState state;
    TicTacToeProblem game;
    TicTacToeProblem::ValueVector rewards;
    static mcts::Solver<TicTacToeProblem, UCB1SelectionPolicy<float>, RolloutPolicy<ttt::TicTacToePolicy> > solver;
    solver.parameter().numIterations = 1000;

    while (!game.isTerminal(state))
    {
        if ((state.getCurrentPlayer() == 0 && !switch_players) || (state.getCurrentPlayer() == 1 && switch_players))
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
        return 1 - switch_players;
    }
    else if (result == -1)
    {
        return switch_players;
    }
    else
    {
        return -1;
    }
}

int main(int, char**)
{
    uint32_t num_iterations = 1000;

    uint32_t num_wins_1 = 0;
    uint32_t num_wins_2 = 0;
    uint32_t num_draws = 0;

    size_t mctsIterations = 0;

    auto tstart = std::chrono::system_clock::now();
    for (size_t i = 0; i < num_iterations; ++i)
    {
        auto t1 = std::chrono::system_clock::now();
        int retval = testMCTSTicTacToe(i % 2 == 0, mctsIterations);
        if (retval == 0)
        {
            num_wins_1++;
        }
        else if (retval == 1)
        {
            num_wins_2++;
        }
        else
        {
            num_draws++;
        }
        auto t2 = std::chrono::system_clock::now();

        auto dt = t2 - t1;

        // print formatted date
        std::cout << "Game # " << i << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(dt).count()
                  << "ms" << std::endl;
    }

    auto tend = std::chrono::system_clock::now();

    auto dttotal = tend - tstart;

    // print formatted date
    std::cout << "All games took " << std::chrono::duration_cast<std::chrono::milliseconds>(dttotal).count()
              << "ms, which is a mean duration of "
              << double(std::chrono::duration_cast<std::chrono::milliseconds>(dttotal).count()) / num_iterations
              << "ms. and a duration of "
              << double(std::chrono::duration_cast<std::chrono::microseconds>(dttotal).count()) / mctsIterations
              << "us per mcts iteration" << std::endl;

    std::cout << "Player 1: " << double(num_wins_1) / double(num_iterations) * 100 << "%\n";
    std::cout << "Player 2: " << double(num_wins_2) / double(num_iterations) * 100 << "%\n";
    std::cout << "Draws:    " << double(num_draws) / double(num_iterations) * 100 << "%\n";
}