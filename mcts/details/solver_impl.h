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

#include "../solver.h"

#include <iomanip>
#include <iostream>

namespace mcts {

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
typename Solver<ProblemType, SelectionPolicy, RolloutPolicy>::ActionType
Solver<ProblemType, SelectionPolicy, RolloutPolicy>::run(const ProblemType& problem, const StateType& root)
{
    running_ = true;
    init(problem, root);

    while (currentIteration_ < params_.numIterations)
    {
        currentIteration_++;
        iteration();
    }

    running_ = false;

    return currentBestAction();
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
typename Solver<ProblemType, SelectionPolicy, RolloutPolicy>::ActionType
Solver<ProblemType, SelectionPolicy, RolloutPolicy>::runFromExistingTree(NodeId newRoot)
{
    running_ = true;
    tree_ = tree_.subTree(newRoot);
    currentIteration_ = 0;

    while (currentIteration_ < params_.numIterations)
    {
        currentIteration_++;
        iteration();
    }

    running_ = false;
    return currentBestAction();
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
NodeId Solver<ProblemType, SelectionPolicy, RolloutPolicy>::selection()
{
    const Node* currentNode = &tree_.root();
    NodeId currentNodeId{0};

    // Selection
    while (true)
    {
        auto selectedNodeId = selectionOnce(*currentNode);
        if (selectedNodeId == INVALID_NODE)
        {
            return currentNodeId;
        }
        currentNodeId = selectedNodeId;
        currentNode = &tree_[selectedNodeId];
    }
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
NodeId Solver<ProblemType, SelectionPolicy, RolloutPolicy>::selectionOnce(const Solver::Node& node)
{
    if (node.isTerminal() || node.isLeaf())
    {
        return INVALID_NODE;
    }
    auto bestChild = selectionPolicy_.selectSuccessor(node);
    assert(bestChild != std::numeric_limits<size_t>::max());
    auto selectedNodeId = tree_[node.outgoingEdges[bestChild]].child;
    return selectedNodeId;
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::expansion(NodeId id)
{
    Node& currentNode = tree_[id];
    expansion(currentNode);
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::expansion(const Node& currentNode,
                                                                    Solver::DecisionNode& decNode)
{
    assert(!decNode.actions.empty());

    for (const auto& action : decNode.actions)
    {
        auto newState = currentNode.state;
        ValueVector rewards = currentNode.problem.performAction(action, newState);

        Node newNode(currentNode.problem, newState);
        newNode.nodeValue = currentNode.nodeValue + rewards;
        auto [nodeId, edgeId] = tree_.insert(currentNode.nodeId, std::move(newNode));
        (void)edgeId;
        // Rollout to gain an estimate of the value of that node
        auto values = rollout(nodeId);
        // Backpropagate
        backpropagate(nodeId, values);
    }
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::expansion(Solver::Node& currentNode,
                                                                    Solver::ChanceNode& chanceNode)
{
    assert(!chanceNode.events.empty());

    if constexpr (ProblemType::hasChanceEvents)
    {
        ValueVector values{};
        for (const auto& event : chanceNode.events)
        {
            auto newState = currentNode.state;
            ValueVector rewards = currentNode.problem.performChanceEvent(event.second, newState);

            Node newNode(currentNode.problem, newState);
            newNode.nodeValue = currentNode.nodeValue + rewards;
            auto [nodeId, edgeId] = tree_.insert(currentNode.nodeId, std::move(newNode));
            (void)edgeId;
            // Rollout to gain an estimate of the value of that node
            values = values + event.first * rollout(nodeId);
        }
        // Backpropagate
        backpropagate(currentNode.nodeId, values);
    }
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::expansion(Solver::Node& currentNode)
{
    assert(!currentNode.isTerminal());
    currentNode.visit([this](Solver::Node& node, auto& subNode) { return this->expansion(node, subNode); });
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
typename Solver<ProblemType, SelectionPolicy, RolloutPolicy>::ValueVector
Solver<ProblemType, SelectionPolicy, RolloutPolicy>::rollout(NodeId nodeId)
{
    auto& currentNode = tree_[nodeId];
    return currentNode.nodeValue + rolloutPolicy_.rollout(currentNode.state, currentNode.problem);
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
std::vector<std::pair<typename ProblemType::ActionType, Statistic<typename ProblemType::ValueType> > >
Solver<ProblemType, SelectionPolicy, RolloutPolicy>::getTopLevelUtilities() const
{
    std::vector<std::pair<typename ProblemType::ActionType, Statistic<typename ProblemType::ValueType> > > retval{};

    const auto& rootNode = tree_.root();

    const auto& decisionNode = std::get<0>(rootNode.payload);
    const auto& actionMap = decisionNode.statistics.getStatistics();

    for (size_t ac = 0; ac < actionMap.size(); ac++)
    {
        if (actionMap[ac].count() > 0)
        {
            retval.emplace_back(decisionNode.actions[ac], actionMap[ac]);
        }
    }

    return retval;
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::printTopLevelUtilities() const
{
    auto topLevelUtilities = getTopLevelUtilities();

    for (auto& [action, stat] : topLevelUtilities)
    {
        std::cout << "Action " << action << " has a mean value of " << stat.value() << "[" << stat.count() << "]\n";
    }
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::init(const ProblemType& problem, const StateType& root)
{
    tree_.reserve(params_.numIterations * std::max(ProblemType::maxNumActions, ProblemType::maxChanceEvents));
    tree_.setRoot(Node{problem, root, DecisionNode{problem, root}});
    currentIteration_ = 0;
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::iteration()
{
    // Selection
    auto selectedNodeId = selection();
    assert(selectedNodeId != INVALID_NODE);
    const auto& selectedNode = tree_[selectedNodeId];
    if (selectedNode.isTerminal())
    {
        backpropagate(selectedNodeId, selectedNode.nodeValue);
    }
    else
    {
        // expand all actions/chance events for this particular node to gain an estimate
        expansion(selectedNodeId);
    }
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::backpropagate(const NodeId& expandedNode,
                                                                        const Solver::ValueVector& values)
{
    auto currentNodeId = expandedNode;
    while (true)
    {
        if (currentNodeId == ROOT_NODE)
        {
            break;
        }

        // traverse the tree upward, as its a tree only one parent can be present
        const auto& currentNode = tree_[currentNodeId];
        const auto& edge = tree_[currentNode.incomingEdge];
        const auto parentNodeId = edge.parent;
        auto& parentNode = tree_[parentNodeId];
        visitBackpropagate(parentNode, edge, values);
        currentNodeId = parentNodeId;
    }
}  // namespace mcts

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::visitBackpropagate(Solver::ChanceNode&, const Edge&,
                                                                             const ValueVector&)
{
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::visitBackpropagate(Solver::DecisionNode& node,
                                                                             const Edge& edge,
                                                                             const ValueVector& values)
{
    if constexpr (ProblemType::numPlayers > 1)
    {
        node.statistics.visitWithValue(edge.index, values[node.playerId]);
    }
    else
    {
        assert(!std::isnan(values));
        node.statistics.visitWithValue(edge.index, values);
    }
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
void Solver<ProblemType, SelectionPolicy, RolloutPolicy>::visitBackpropagate(Solver::Node& node, const Edge& edge,
                                                                             const ValueVector& values)
{
    node.visit([&](Solver::Node&, auto& subNode) { this->visitBackpropagate(subNode, edge, values); });
}

template <typename ProblemType, typename SelectionPolicy, typename RolloutPolicy>
typename Solver<ProblemType, SelectionPolicy, RolloutPolicy>::ActionType
Solver<ProblemType, SelectionPolicy, RolloutPolicy>::currentBestAction() const
{
    const auto& rootNode = tree_.root();
    const auto& decisionNode = std::get<0>(rootNode.payload);
    const auto& actionMap = decisionNode.statistics.getStatistics();

    size_t bestAction = std::numeric_limits<uint8_t>::max();
    ValueType bestValue = std::numeric_limits<ValueType>::lowest();
    for (size_t ac = 0; ac < actionMap.size(); ac++)
    {
        if (!actionMap[ac].visited())
        {
            continue;
        }
        if (actionMap[ac].value() > bestValue)
        {
            bestValue = actionMap[ac].value();
            bestAction = ac;
        }
    }
    return decisionNode.actions[bestAction];
}

}  // namespace mcts
