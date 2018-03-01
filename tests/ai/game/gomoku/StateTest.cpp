#include <gmock/gmock.h>
#include <ai/game/gomoku/State.hpp>

namespace ai {
namespace game {
namespace gomoku {

void assert_action_eq(Action value, Action compared) {
    ASSERT_EQ(value.x, compared.x);
    ASSERT_EQ(value.y, compared.y);
}

TEST(State, default_values) {
    ASSERT_EQ(Cell{}, Cell::NONE);
    ASSERT_EQ(bool{}, false);
}

TEST(State, legalActions) {
    State state;
    auto actions = state.legalActions();
    ASSERT_EQ(actions.size(), 1);
    assert_action_eq(actions[0], {0, 0});
}

} // namespace gomoku
} // namespace game
} // namespace ai

