#include "games/tic_tac_toe/tic_tac_toe.h"
#include "mcts/tree.h"

#include <gtest/gtest.h>

using namespace ttt;
using TTTTree = mcts::Tree<ttt::TicTacToeProblem>;

TEST(Tree, Init)
{
    TTTTree tree{};

    TicTacToeState state{};
    TicTacToeProblem p{};
    tree.setRoot(TTTTree::Node(p, state, TTTTree::Node::DecisionNode(p, state)));
    TTTTree::Node node(p, state, TTTTree::Node::DecisionNode(p, state));

    tree.reserve(100);
    auto rootId = tree.root().nodeId;
    tree.insert(rootId, node);
    tree.insert(rootId, node);
    tree.insert(rootId, node);
    tree.insert(rootId, node);
    tree.insert(rootId, node);
    tree.insert(rootId, node);

    ASSERT_EQ(tree[rootId].outgoingEdges.size(), 6);
    ASSERT_EQ(tree.nodeCount(), 7);
}