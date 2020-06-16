#pragma once

#include "tree.h"

#include <fstream>
#include <sstream>

namespace mcts::dot {

template <typename ProblemType>
void exportEdge(const typename Tree<ProblemType>::Edge& edge, const typename Tree<ProblemType>::Node node,
                const typename Tree<ProblemType>::Node::DecisionNode& parentNode, std::ostream& stream)
{
    using namespace std;
    const std::string edgeLabel = node.problem.actionToString(node.state, parentNode.actions[edge.index]);
    stream << edge.parent.get() << " -> " << edge.child.get() << "[label=\"" << edgeLabel << "\"];\n";
}

template <typename ProblemType>
void exportEdge(const typename Tree<ProblemType>::Edge& edge, const typename Tree<ProblemType>::Node node,
                const typename Tree<ProblemType>::Node::ChanceNode& parentNode, std::ostream& stream)
{
    if constexpr (ProblemType::hasChanceEvents)
    {
        using namespace std;
        const std::string edgeLabel = node.problem.eventToString(node.state, parentNode.events[edge.index].second);
        const std::string tailLabel = to_string(parentNode.events[edge.index].first);

        stream << edge.parent.get() << " -> " << edge.child.get() << "[label=\"" << edgeLabel << "\", taillabel=\""
               << tailLabel << "\"];\n";
    }
}

template <typename ProblemType>
void exportEdge(const typename Tree<ProblemType>::Edge& edge, const typename Tree<ProblemType>::Node& parentNode,
                std::ostream& stream)
{
    assert(edge.parent == parentNode.nodeId);
    parentNode.visit([&edge, &stream](const typename Tree<ProblemType>::Node& par, const auto& node) {
        exportEdge<ProblemType>(edge, par, node, stream);
    });
}

template <typename ProblemType>
void exportNode(const typename Tree<ProblemType>::Node& node, const Tree<ProblemType>& tree, std::ostream& stream)
{
    using namespace std;

    std::stringstream str{};
    node.state.writeToStream(str);
    const std::string shape = node.isLeaf() ? "circle" : (node.isDecision() ? "box" : "diamond");
    const std::string label = str.str();
    stream << node.nodeId.get() << " [shape=" << shape << ", label=\"" << label << "\"];\n";

    for (auto& edgeId : node.outgoingEdges)
    {
        const auto& edge = tree[edgeId];
        exportEdge<ProblemType>(edge, node, stream);
    }
}

template <typename ProblemType>
void exportTreeToDot(const Tree<ProblemType>& tree, std::ostream& stream)
{
    stream << "digraph mcts { \n";
    for (const auto& node : tree.nodes())
    {
        exportNode<ProblemType>(node, tree, stream);
    }

    stream << "}\n";
}

template <typename ProblemType>
void exportTreeToDot(const Tree<ProblemType>& tree, const std::string_view filename)
{
    std::ofstream file(filename);
    exportTreeToDot(tree, file);
}

}