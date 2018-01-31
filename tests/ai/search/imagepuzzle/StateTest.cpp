#include <gmock/gmock.h>
#include <ai/search/imagepuzzle/State.hpp>

namespace ai {
namespace search {
namespace imagepuzzle {

TEST(State, accessor_default_constructor) {
    State state;
    ASSERT_EQ(state(1, 1), 0);
    ASSERT_EQ(state(3, 3), 8);
    ASSERT_EQ(state(2, 2), 4);
}

TEST(State, initializer_list_and_equals) {
    State state{1, 0, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(state(1, 2), 0);
    ASSERT_EQ(state(1, 1), 1);

    ASSERT_TRUE(state == state);
    State state2{1, 2, 0, 3, 4, 5, 6, 7, 8};
    ASSERT_FALSE(state == state2);
    ASSERT_TRUE(state != state2);

    ASSERT_EQ(state2(1, 3), 0);
}

bool contains(const std::vector<Action>& actions, Action action) {
    auto it = std::find(actions.begin(), actions.end(), action);
    return it != actions.end();
}

TEST(State, legalActions) {
    State state;
    auto actions = state.legalActions();
    ASSERT_EQ(actions.size(), 2);
    ASSERT_TRUE(contains(actions, Action::DOWN));
    ASSERT_TRUE(contains(actions, Action::RIGHT));

    auto tmp_state = state;
    ASSERT_EQ(tmp_state(2, 1), 3);

    state.move(Action::RIGHT);
    ASSERT_EQ(state(1, 2), 0);
    ASSERT_EQ(state(1, 1), 1);

    actions = state.legalActions();
    ASSERT_EQ(actions.size(), 3);
    ASSERT_TRUE(contains(actions, Action::DOWN));
    ASSERT_TRUE(contains(actions, Action::RIGHT));
    ASSERT_TRUE(contains(actions, Action::LEFT));

    state.move(Action::DOWN);
    actions = state.legalActions();
    ASSERT_EQ(actions.size(), 4);

    State compared{1, 4, 2, 3, 0, 5, 6, 7, 8};
    ASSERT_TRUE(state == compared);
}

} // namespace imagepuzzle
} // namespace search
} // namespace ai

