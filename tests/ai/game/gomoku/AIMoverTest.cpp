#include <gmock/gmock.h>
#include <ai/game/gomoku/AIMover.hpp>
#include <chrono>

namespace ai {
namespace game {
namespace gomoku {

using namespace std::chrono;

TEST(AIMover, moved) {
    State state{Cell::AI};
    AIMover mover{state};
    mover.next_move_in_background();
    ASSERT_EQ(mover.moved(), false);
    ASSERT_EQ(mover.thinking(), true);

    std::this_thread::sleep_for(10ms);
    ASSERT_EQ(mover.thinking(), false);
    ASSERT_EQ(mover.moved(), true);
    auto recent = mover.recent_move();
    ASSERT_EQ(recent.x, 0);
    ASSERT_EQ(recent.y, 0);
}

} // namespace gomoku
} // namespace game
} // namespace ai

