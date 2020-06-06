#include <gtest/gtest.h>

#include "2048.h"

struct TestData
{
    g2048::Board source;
    g2048::Actions action;
    g2048::Board target;
};

class G2048Test : public testing::TestWithParam<TestData>
{
};

TEST_P(G2048Test, Action)
{
    auto params = GetParam();

    g2048::G2048State state(params.source);
    g2048::G2048Problem problem;
    std::cout << "Before: --------------------\n";
    state.print();
    problem.performAction(params.action, state, state);
    std::cout << "After: #####################\n";
    state.print();
    ASSERT_EQ(state.board(), params.target);
}

INSTANTIATE_TEST_CASE_P(
    , G2048Test,
    testing::ValuesIn({TestData{.source = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 0, 0}, {0, 0, 0, 0}}},
                                .action = g2048::DOWN,
                                .target = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 2}, {0, 0, 0, 0}}}},
                       TestData{.source = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {2, 2, 0, 0}, {0, 0, 0, 0}}},
                                .action = g2048::DOWN,
                                .target = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 3}, {0, 0, 0, 0}}}},
                       TestData{.source = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {4, 2, 2, 0}, {0, 0, 0, 0}}},
                                .action = g2048::DOWN,
                                .target = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 4, 3}, {0, 0, 0, 0}}}},
                       TestData{.source = {{{0, 0, 0, 0}, {0, 0, 0, 4}, {2, 0, 4, 2}, {0, 2, 4, 8}}},
                                .action = g2048::DOWN,
                                .target = {{{0, 0, 0, 0}, {0, 0, 0, 4}, {0, 2, 4, 2}, {0, 2, 4, 8}}}}}));

