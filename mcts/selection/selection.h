#pragma once
#include <cmath>
#include <variant>
#include "../solver.h"

#include <random>

namespace mcts {
template<class T>
class SelectionPolicy
{
  public:
    SelectionPolicy() = default;

    template <typename Node>
    size_t selectSuccessor(const Node& currentNode)
    {
        return currentNode.visit(*this);
    }

    template <typename Node>
    size_t operator()(const Node&, const typename Node::ChanceNode& chance)
    {
        static thread_local std::minstd_rand0 engine{};
        static thread_local std::uniform_real_distribution<float> dist{0, 1.0f};

        float randVal = dist(engine);

        size_t counter = 0;
        for (const auto& ev : chance.remainingEvents)
        {
            if (randVal < ev.first)
            {
                return counter;
            }
            randVal -= ev.first;
            counter++;
        }
        return chance.remainingEvents.size() - 1;
    }

    template <typename Node>
    size_t operator()(const Node& node, const typename Node::DecisionNode& decision)
    {
        return static_cast<T*>(this)->selectDecisionNodeSuccessor(node, decision);
    }

  private:
    std::minstd_rand0 engine{};
    std::uniform_real_distribution<float> dist{0, 1.0f};
};
}