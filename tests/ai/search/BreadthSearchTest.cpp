#include <gmock/gmock.h>
#include <ai/search/BreadthSearch.hpp>
#include <ai/search/imagepuzzle/Problem.hpp>

namespace ai {
namespace search {

TEST(BreadthSearch, solving) {
    using namespace imagepuzzle;

    // State begin{7, 2, 4, 5, 0, 6, 8, 3, 1};
    State begin{1, 4, 2, 3, 0, 5, 6, 7, 8};
    State end{0, 1, 2, 3, 4, 5, 6, 7, 8};
    BreadthSearch<Problem> solver(begin, end);
    auto actions = solver.solve();
    ASSERT_NE(actions.size(), 0);
    std::vector<Action> compared{Action::UP, Action::LEFT};
    ASSERT_EQ(actions, compared);
}

} // namespace search
} // namespace ai

