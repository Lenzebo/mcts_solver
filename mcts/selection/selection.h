// MIT License
//
// Copyright (c) 2020 Lenzebo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cassert>
#include <cmath>
#include <random>
#include <variant>

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
}  // namespace mcts