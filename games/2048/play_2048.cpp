#include "2048.h"
#include "mcts/selection/ucb1.h"
#include "mcts/solver.h"

#include <chrono>
#include <fstream>
#include <iostream>

using namespace mcts;

int main(int, char**)
{
    g2048::G2048State state;
    g2048::G2048Problem game;

    float totalValue = {0};
    while (!game.isTerminal(state))
    {
        if (state.getCurrentPlayer() == 0 && game.getNextStageType(state) == mcts::StageType::DECISION)
        {
            state.print();
            std::cout << "\n";
            int32_t action_id{};
            auto possible_actions = game.getAvailableActions(state);
            size_t i = 0;
            for (auto ac : possible_actions)
            {
                std::cout << i << ": " << game.actionToString(state, ac) << "\n";
                ++i;
            }
            std::cout << "Which Action should be performed?\n";
            std::cin >> action_id;
            totalValue += game.performAction(possible_actions[action_id], state);
            std::cout << "Value: " << totalValue << "\n";
        }
        else
        {
            auto possible_events = game.getAvailableChanceEvents(state);
            size_t i = 0;
            for (auto ac : possible_events)
            {
                std::cout << i << ": (" << int(ac.second.x) << ", " << int(ac.second.y) << ")->" << int(ac.second.value)
                          << " with a probability of " << ac.first << "\n";
                ++i;
            }
            totalValue += game.performRandomChanceEvent(state);
        }
    }
}