#pragma once

#include "tree.h"
#include "node_statistic.h"
#include "selection/ucb1.h"
#include "rollout/random_rollout.h"

#include "utils/named_type.h"
#include "utils/max_size_vector.h"

#include <cmath>
#include <random>
#include <variant>
#include <atomic>

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
        /// maximal number of iterations the algorithm should run
        size_t numIterations = 10000;
    };

    Solver() = default;
    Solver(SelectionPolicy&& policy) : selectionPolicy_(policy) {}
    Solver(SelectionPolicy&& policy, RolloutPolicy&& rolloutPolicy)
        : selectionPolicy_(policy), rolloutPolicy_(rolloutPolicy)
    {
    }

    ActionType run(const ProblemType& problem, const StateType& root);
    ActionType runFromExistingTree(NodeId newRoot);

    Parameter& parameter() { return params_; }
    const Parameter& parameter() const { return params_; }

    bool isRunning() const { return running_; }
    size_t currentIteration() const { return currentIteration_; }

    void printTopLevelUtilities() const;
    const TreeType tree() const { return tree_; }

  private:
    void init(const ProblemType& problem, const StateType& root);
    void iteration();
    ActionType currentBestAction() const;

    NodeId selection();
    NodeId selectionOnce(const Node& node);

    void expansion(NodeId current_node);
    void expansion(Node& node);
    void expansion(const Node& node, DecisionNode& decNode);
    void expansion(Node& node, ChanceNode& chanceNode);

    ValueVector rollout(NodeId current_node);

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
