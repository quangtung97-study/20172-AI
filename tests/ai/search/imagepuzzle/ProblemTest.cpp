#include <gmock/gmock.h>
#include <ai/search/imagepuzzle/Problem.hpp>

namespace ai {
namespace search {
namespace imagepuzzle {

TEST(Problem, heuristic_function_simple) {
    State begin{1, 2, 0, 3, 4, 5, 6, 7, 8};
    State end{0, 1, 2, 3, 4, 5, 6, 7, 8};
    Node node{begin};
    
    Problem problem{begin, end};
    ASSERT_EQ(problem.h(&node), 2);
}

TEST(Problem, heuristic_function_complex) {
    State begin{7, 2, 4, 5, 0, 6, 8, 3, 1};
    State end{0, 1, 2, 3, 4, 5, 6, 7, 8};
    Node node{begin};
    
    Problem problem{begin, end};
    ASSERT_EQ(problem.h(&node), 18);
}

TEST(Problem, goal_function) {
    State begin{7, 2, 4, 5, 0, 6, 8, 3, 1};
    State end{0, 1, 2, 3, 4, 5, 6, 7, 8};
    Node root{begin};
    Node second{&root, Action::LEFT};
    Node third{&second, Action::UP};

    Problem problem{begin, end};
    ASSERT_EQ(problem.g(&third), 2);
    ASSERT_EQ(problem.g(&second), 1);
    ASSERT_EQ(problem.g(&root), 0);
}

} // namespace imagepuzzle
} // namespace search
} // namespace ai
