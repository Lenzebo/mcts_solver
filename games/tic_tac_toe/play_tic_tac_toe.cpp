#include <iostream>
#include <fstream>
#include <chrono>

#include "tic_tac_toe.h"
#include "mcts/solver.h"

#include "mcts/tree_export.h"

using namespace mcts;
using namespace ttt;

void testGameStateTicTacToe()
{
    TicTacToeState state;
    TicTacToeProblem game;

    while (!game.isTerminal(state))
    {
        auto possible_actions = game.getAvailableActions(state);

        if (state.getCurrentPlayer() == 0)
        {
            state.print();
            std::cout << "\n";
            int32_t action_id{};
            size_t i = 0;
            for (auto ac : possible_actions)
            {
                std::cout << i << ": " << game.actionToString(state, ac) << "\n";
                ++i;
            }
            std::cout << "Which Action should be performed?\n";
            std::cin >> action_id;
            auto value = game.performAction(possible_actions[action_id], state);
            std::cout << "Value: " << value[0] << ", " << value[1] << "\n";
        }
        else
        {
            state.print();
            std::cout << "\n";
            mcts::Solver<TicTacToeProblem, UCB1SelectionPolicy<float>, RolloutPolicy<ttt::TicTacToePolicy> > solver;
            solver.parameter().numIterations = 100000;
            auto action = solver.run(game, state);
            solver.printTopLevelUtilities();
            auto value = game.performAction(action, state);
            mcts::dot::exportTreeToDot(solver.tree(), "tree.dot");
            std::cout << "Value: " << value[0] << ", " << value[1] << "\n";
        }
    }
}


int main(int, char**)
{
    testGameStateTicTacToe();
    return 0;
}