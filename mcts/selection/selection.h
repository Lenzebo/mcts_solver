#pragma once
#include <cmath>
#include <variant>
#include "../solver.h"

#include <random>

namespace mcts {
template <class T>
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
        assert(!chance.events.empty());

        float randVal = dist_(engine_);

        size_t counter = 0;
        for (const auto& ev : chance.events)
        {
            if (randVal < ev.first)
            {
                return counter;
            }
            randVal -= ev.first;
            counter++;
        }
        return chance.events.size() - 1;
    }

    template <typename Node>
    size_t operator()(const Node& node, const typename Node::DecisionNode& decision)
    {
        return static_cast<T*>(this)->selectDecisionNodeSuccessor(node, decision);
    }

    void seed(size_t seed) { engine_.seed(seed); }

  private:
    std::minstd_rand0 engine_{std::random_device{}()};
    std::uniform_real_distribution<float> dist_{0, 1.0f};
};
}