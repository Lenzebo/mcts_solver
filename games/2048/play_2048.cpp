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
            int32_t actionId{};
            auto possibleActions = game.getAvailableActions(state);
            size_t i = 0;
            for (auto ac : possibleActions)
            {
                std::cout << i << ": " << game.actionToString(state, ac) << "\n";
                ++i;
            }
            std::cout << "Which Action should be performed?\n";
            std::cin >> actionId;
            totalValue += game.performAction(possibleActions[actionId], state);
            std::cout << "Value: " << totalValue << "\n";
        }
        else
        {
            auto possibleEvents = game.getAvailableChanceEvents(state);
            size_t i = 0;
            for (auto ac : possibleEvents)
            {
                std::cout << i << ": (" << int(ac.second.x) << ", " << int(ac.second.y) << ")->" << int(ac.second.value)
                          << " with a probability of " << ac.first << "\n";
                ++i;
            }
            totalValue += game.performRandomChanceEvent(state);
        }
    }
}