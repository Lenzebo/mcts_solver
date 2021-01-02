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
    if constexpr (ProblemType::HAS_CHANCE_EVENTS)
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
void exportTreeToDot(const Tree<ProblemType>& tree, const std::string filename)
{
    std::ofstream file(filename);
    exportTreeToDot(tree, file);
}

}  // namespace mcts::dot