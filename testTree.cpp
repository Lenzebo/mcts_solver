#include "mcts/tree.h"

#include "games/GameTicTacToe.h"

#include <gtest/gtest.h>

using TTTTree = mcts::Tree<TicTacToeProblem>;

TEST(Tree, Init)
{
    TTTTree tree {};

    TicTacToeState state{};
    TicTacToeProblem p{};
    tree.setRoot(TTTTree::Node(p,state,TTTTree::DecisionNode(p,state)));
    TTTTree::Node node (p,state,TTTTree::DecisionNode(p,state));

    auto rootId = tree.root().nodeId;
    tree.insert(rootId, node;
    tree.insert(rootId, node;
    tree.insert(rootId, node;
    tree.insert(rootId, node;
    tree.insert(rootId, node;
    tree.insert(rootId, node;

    ASSERT_EQ(tree[rootId].outgoingEdges.size() , 6);
    ASSERT_EQ(tree.nodeCount(), 7);
}