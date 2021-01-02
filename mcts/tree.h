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
#include "zbo/max_size_vector.h"
#include "zbo/named_type.h"

#include <variant>
#include <vector>

namespace mcts {

struct NodeId : public zbo::NamedType<size_t, NodeId>, zbo::EqualityComparable<NodeId>
{
    using NamedType::NamedType;
};

struct EdgeId : public zbo::NamedType<size_t, NodeId>, zbo::EqualityComparable<NodeId>
{
    using NamedType::NamedType;
};

constexpr EdgeId ROOT_EDGE{std::numeric_limits<size_t>::max()};
constexpr NodeId INVALID_NODE{std::numeric_limits<size_t>::max()};
constexpr NodeId ROOT_NODE{0};

template <class ProblemType>
struct Tree
{
    using ValueType = typename ProblemType::ValueType;
    using ValueVector = typename ProblemType::ValueVector;
    using StateType = typename ProblemType::StateType;
    using ActionType = typename ProblemType::ActionType;
    using ChanceEventType = typename ProblemType::ChanceEventType;
    using ChanceEventWithProbability = std::pair<float, ChanceEventType>;

    struct Node
    {
        /**
         * @brief A player in the game has to make a decision out of a set of possible actions that he can perform based
         * on the current state
         */
        struct DecisionNode
        {
            explicit DecisionNode(const ProblemType& p, const StateType& s) noexcept
            {
                actions = p.getAvailableActions(s);
                playerId = s.getCurrentPlayer();
            }
            NodeStatistic<ValueType, ProblemType::MAX_NUM_ACTIONS> statistics{};
            zbo::MaxSizeVector<ActionType, ProblemType::MAX_NUM_ACTIONS> actions{};
            uint8_t playerId{};
        };

        /**
         * @brief A random element in the game/problem is happening with a known probability
         * This could be for example the revealing of public cards or the roll of a dice
         */
        struct ChanceNode
        {
            explicit ChanceNode(const ProblemType& p, const StateType& s) noexcept
            {
                if constexpr (ProblemType::HAS_CHANCE_EVENTS)
                {
                    events = p.getAvailableChanceEvents(s);
                }
            }
            zbo::MaxSizeVector<ChanceEventWithProbability, ProblemType::MAX_CHANCE_EVENTS> events;
        };

        using PayloadType = std::variant<DecisionNode, ChanceNode>;

        [[nodiscard]] static PayloadType payloadFromState(const ProblemType& p, const StateType& s)
        {
            if constexpr (ProblemType::HAS_CHANCE_EVENTS)
            {
                switch (p.getNextStageType(s))
                {
                    case mcts::StageType::DECISION:
                        return DecisionNode(p, s);
                    case mcts::StageType::CHANCE:
                        return ChanceNode(p, s);
                }
            }
            return DecisionNode(p, s);
        }

        explicit Node(const ProblemType& p, const StateType& s) noexcept
            : state(s), problem(p), payload(payloadFromState(p, s))
        {
        }

        explicit Node(const ProblemType& p, const StateType& s, PayloadType payload) noexcept
            : state(s), problem(p), payload(std::move(payload))
        {
        }

        [[nodiscard]] constexpr bool isTerminal() const { return problem.isTerminal(state); }
        [[nodiscard]] constexpr bool isLeaf() const { return outgoingEdges.empty(); }

        [[nodiscard]] constexpr bool isChance() const { return std::holds_alternative<ChanceNode>(payload); }
        [[nodiscard]] constexpr bool isDecision() const { return std::holds_alternative<DecisionNode>(payload); }

        template <class Visitor>
        inline constexpr decltype(auto) visit(Visitor&& visitor)
        {
            return std::visit([&](auto& subNode) { return visitor(*this, subNode); }, payload);
        }

        template <class Visitor>
        inline constexpr decltype(auto) visit(Visitor&& visitor) const
        {
            return std::visit([&](const auto& subNode) { return visitor(*this, subNode); }, payload);
        }

        NodeId nodeId{};
        EdgeId incomingEdge{ROOT_EDGE};
        zbo::MaxSizeVector<EdgeId, std::max(ProblemType::MAX_NUM_ACTIONS, ProblemType::MAX_CHANCE_EVENTS)>
            outgoingEdges{};

        StateType state{};
        const ProblemType& problem;
        /// saves the value (for each player) up to this node without looking into the future
        ValueVector nodeValue{};

        /// Extra payload for type of nodes
        PayloadType payload;
    };

    struct Edge
    {
        Edge() : parent(INVALID_NODE), child(INVALID_NODE) {}
        size_t index{};  ///< the index in the parents outgoing edges list
        NodeId parent{};
        NodeId child{};
    };

    Tree() = default;

    explicit Tree(Node root, size_t expectedNodes = 1)
    {
        reserve(expectedNodes);
        setRoot(root);
    }

    void clear()
    {
        nodes_.clear();
        edges_.clear();
    }

    void reserve(size_t expectedNodes)
    {
        nodes_.reserve(expectedNodes);
        edges_.reserve(expectedNodes - 1);
    }

    void setRoot(Node root)
    {
        clear();
        root.outgoingEdges = {};
        root.incomingEdge = ROOT_EDGE;
        root.nodeId = ROOT_NODE;
        nodes_.push_back(root);
    }

    [[nodiscard]] bool contains(NodeId node) const { return node.get() < nodes_.size(); }

    [[nodiscard]] Tree subTree(NodeId parent) const
    {
        if (!contains(parent))
        {
            return {};
        }

        Tree tree{};
        tree.reserve(nodes_.capacity());
        tree.setRoot((*this)[parent]);
        insertExpandSubTree(tree, parent, ROOT_NODE);
        tree.root().nodeValue = {};
        return tree;
    }

    [[nodiscard]] auto begin() const { return nodes_.begin(); }
    [[nodiscard]] auto end() const { return nodes_.end(); }

    [[nodiscard]] size_t capacity() const { return nodes_.capacity(); }
    [[nodiscard]] size_t nodeCount() const { return nodes_.size(); }

    [[nodiscard]] Node& operator[](NodeId node) { return nodes_[node.get()]; };
    [[nodiscard]] const Node& operator[](NodeId node) const { return nodes_[node.get()]; };
    [[nodiscard]] Edge& operator[](EdgeId edge) { return edges_[edge.get()]; };
    [[nodiscard]] const Edge& operator[](EdgeId edge) const { return edges_[edge.get()]; };

    [[nodiscard]] const Node& root() const { return nodes_[0]; }
    [[nodiscard]] Node& root() { return nodes_[0]; }

    std::pair<NodeId, EdgeId> insert(NodeId parent, Node newNode)
    {
        assert(nodes_.capacity() > nodes_.size());
        assert(edges_.capacity() > edges_.size());

        // first add node to list of nodes_
        NodeId newId{nodes_.size()};
        EdgeId newEdgeId{edges_.size()};
        newNode.nodeId = newId;
        newNode.incomingEdge = newEdgeId;
        newNode.outgoingEdges = {};
        nodes_.push_back(std::move(newNode));

        auto& parentNode = (*this)[parent];
        Edge newEdge{};
        newEdge.parent = parent;
        newEdge.child = newId;
        newEdge.index = parentNode.outgoingEdges.size();
        parentNode.outgoingEdges.push_back(newEdgeId);
        edges_.push_back(std::move(newEdge));

        return {newId, newEdgeId};
    }

    [[nodiscard]] const std::vector<Node>& nodes() const { return nodes_; }
    [[nodiscard]] const std::vector<Edge>& edges() const { return edges_; }

  private:
    void insertExpandSubTree(Tree& tree, NodeId parent, NodeId newParent) const
    {
        for (const EdgeId edge : (*this)[parent].outgoingEdges)
        {
            auto childNodeId = (*this)[edge].child;
            auto [nodeId, edgeId] = tree.insert(newParent, (*this)[childNodeId]);
            tree[nodeId].nodeValue -= tree.root().nodeValue;
            insertExpandSubTree(tree, childNodeId, nodeId);
        }
    }

    std::vector<Node> nodes_;
    std::vector<Edge> edges_;
};

}  // namespace mcts