#include <gmock/gmock.h>
#include <ai/search/DepthSearch.hpp>
#include <ai/search/imagepuzzle/Problem.hpp>

namespace ai {
namespace search {

TEST(DepthSearch, intergration) {
    using namespace imagepuzzle;

    State begin{7, 2, 4, 5, 0, 6, 8, 3, 1};
    State end{0, 1, 2, 3, 4, 5, 6, 7, 8};

    Problem problem{begin, end};
    DepthSearch<Problem> solver{problem};
    auto actions = solver.solve();

    auto state = begin;
    for (auto action: actions)
        state.move(action);
    ASSERT_TRUE(state == end);
}

} // namespace search
} // namespace ai

