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

#include "node_statistic.h"
#include "rollout/random_rollout.h"
#include "selection/ucb1.h"
#include "tree.h"
#include "zbo/max_size_vector.h"
#include "zbo/named_type.h"

#include <atomic>
#include <cmath>
#include <random>
#include <variant>

namespace mcts {

template <typename ProblemType, typename SelectionPolicy = UCB1SelectionPolicy<typename ProblemType::ValueType>,
          typename RolloutPolicy = RandomRolloutPolicy>
class Solver
{
  public:
    using ValueType = typename ProblemType::ValueType;
    using ValueVector = typename ProblemType::ValueVector;
    using StateType = typename ProblemType::StateType;
    using ActionType = typename ProblemType::ActionType;
    using TreeType = Tree<ProblemType>;
    using Node = typename TreeType::Node;
    using Edge = typename TreeType::Edge;
    using DecisionNode = typename Node::DecisionNode;
    using ChanceNode = typename Node::ChanceNode;

  public:
    struct Parameter
    {
        static constexpr size_t DEFAULT_ITERATIONS = 10000;
        /// maximal number of iterations the algorithm should run
        size_t numIterations = DEFAULT_ITERATIONS;
    };

    Solver() = default;
    Solver(SelectionPolicy&& policy) : selectionPolicy_(policy) {}
    Solver(SelectionPolicy&& policy, RolloutPolicy&& rolloutPolicy)
        : selectionPolicy_(policy), rolloutPolicy_(rolloutPolicy)
    {
    }

    ActionType run(const ProblemType& problem, const StateType& root);
    ActionType runFromExistingTree(NodeId newRoot);

    [[nodiscard]] Parameter& parameter() { return params_; }
    [[nodiscard]] const Parameter& parameter() const { return params_; }

    [[nodiscard]] bool isRunning() const { return running_; }
    [[nodiscard]] size_t currentIteration() const { return currentIteration_; }

    void printTopLevelUtilities() const;
    [[nodiscard]] std::vector<std::pair<ActionType, Statistic<ValueType>>> getTopLevelUtilities() const;

    [[nodiscard]] const TreeType& tree() const { return tree_; }

  private:
    void init(const ProblemType& problem, const StateType& root);
    void iteration();
    [[nodiscard]] ActionType currentBestAction() const;

    [[nodiscard]] NodeId selection();
    [[nodiscard]] NodeId selectionOnce(const Node& node);

    void expansion(NodeId currentNode);
    void expansion(Node& node);
    void expansion(const Node& node, DecisionNode& decNode);
    void expansion(Node& node, ChanceNode& chanceNode);

    [[nodiscard]] ValueVector rollout(NodeId currentNode);

    void backpropagate(const NodeId& expandedNode, const ValueVector& values);
    void visitBackpropagate(Node& node, const Edge& edge, const ValueVector& values);
    void visitBackpropagate(DecisionNode& node, const Edge& edge, const ValueVector& values);
    void visitBackpropagate(ChanceNode& node, const Edge& edge, const ValueVector& values);

    bool running_ = false;
    size_t currentIteration_ = 0;
    Parameter params_{};

    TreeType tree_{};
    SelectionPolicy selectionPolicy_{};
    RolloutPolicy rolloutPolicy_{};
};

}  // namespace mcts

#include "details/solver_impl.h"
