#pragma once
#include "utils/named_type.h"
#include "node_statistic.h"
#include "utils/max_size_vector.h"

#include <variant>
#include <vector>

namespace mcts {

using NodeId = NamedType<size_t, struct NodeTag>;
using EdgeId = NamedType<size_t, struct EdgeTag>;

constexpr EdgeId ROOT_EDGE{std::numeric_limits<size_t>::max()};
constexpr NodeId INVALID_NODE{std::numeric_limits<size_t>::max()};

template <class ProblemType>
struct Tree
{
    using ValueType = typename ProblemType::ValueType;
    using ValueVector = typename ProblemType::ValueVector;
    using StateType = typename ProblemType::StateType;
    using ActionType = typename ProblemType::ActionType;
    using ChanceEventType = typename ProblemType::ChanceEventType;

    struct Node
    {
        /**
     * @brief A player in the game has to make a decision out of a set of possible actions that he can perform based on
     * the current state
     */
        struct DecisionNode
        {
            explicit DecisionNode(const ProblemType& p, const StateType& s) noexcept
            {
                remainingActions = p.getAvailableActions(s);
                playerId = s.getCurrentPlayer();
            }
            NodeStatistic<ValueType, ProblemType::maxNumActions> statistics{};
            MaxSizeVector<ActionType, ProblemType::maxNumActions> remainingActions{};
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
                if constexpr (ProblemType::hasChanceEvents)
                {
                    remainingEvents = p.getAvailableChanceEvents(s);
                }
            }
            MaxSizeVector<std::pair<float, ChanceEventType>, ProblemType::maxChanceEvents> remainingEvents;
        };

        using PayloadType = std::variant<DecisionNode, ChanceNode>;

        static PayloadType payloadFromState(const ProblemType& p, const StateType& s)
        {
            if constexpr (ProblemType::hasChanceEvents)
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

        explicit Node(const ProblemType& p, const StateType& s, const PayloadType& payload) noexcept
            : state(s), problem(p), payload(payload)
        {
        }

        [[nodiscard]] bool isTerminal() const { return problem.isTerminal(state); }
        [[nodiscard]] bool isLeaf() const { return outgoingEdges.empty(); }

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
        MaxSizeVector<EdgeId, std::max(ProblemType::maxNumActions, ProblemType::maxChanceEvents)> outgoingEdges{};

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
        root.nodeId = NodeId{0};
        nodes_.push_back(root);
    }

    [[nodiscard]] size_t capacity() const { return nodes_.capacity(); }
    [[nodiscard]] size_t nodeCount() const { return nodes_.size(); }

    Node& operator[](NodeId node) { return nodes_[node.get()]; };
    const Node& operator[](NodeId node) const { return nodes_[node.get()]; };
    Edge& operator[](EdgeId edge) { return edges_[edge.get()]; };
    const Edge& operator[](EdgeId edge) const { return edges_[edge.get()]; };

    const Node& root() const { return nodes_[0]; }
    Node& root() { return nodes_[0]; }

    std::pair<NodeId, EdgeId> insert(NodeId parent, Node newNode)
    {
        // TODO concurrency
        assert(nodes_.capacity() > nodes_.size());
        assert(edges_.capacity() > edges_.size());

        // first add node to list of nodes_
        NodeId newId{nodes_.size()};
        EdgeId newEdgeId{edges_.size()};
        newNode.nodeId = newId;
        newNode.incomingEdge = newEdgeId;
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

  private:
    std::vector<Node> nodes_;
    std::vector<Edge> edges_;
};

}