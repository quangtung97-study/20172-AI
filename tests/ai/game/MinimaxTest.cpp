#include <gmock/gmock.h>
#include <ai/game/Minimax.hpp>
#include <ai/game/tictactoe/State.hpp>

namespace ai {
namespace game {

TEST(Minimax, tictactoe) {
    using tictactoe::State;
    using tictactoe::Action;

    State state{State::PLAYER_AI};

    {
        Minimax<State> solver{state};
        Action next = solver.next_action();
        ASSERT_EQ(next.x(), 1);
        ASSERT_EQ(next.y(), 1);
    }
    state.move(Action(1, 1));
    state.move(Action(1, 3));

    {
        Minimax<State> solver{state};
        Action next = solver.next_action();
        ASSERT_EQ(next.x(), 2);
        ASSERT_EQ(next.y(), 1);
    }
    state.move(Action(2, 1));
    state.move(Action(3, 1));

    {
        Minimax<State> solver{state};
        Action next = solver.next_action();
        ASSERT_EQ(next.x(), 2);
        ASSERT_EQ(next.y(), 2);
    }
    state.move(Action(2, 2));
    state.move(Action(2, 3));

    {
        Minimax<State> solver{state};
        Action next = solver.next_action();
        ASSERT_EQ(next.x(), 3);
        ASSERT_EQ(next.y(), 3);
    }
    state.move(Action(3, 3));

    State::Player win_player;
    float utility;
    ASSERT_TRUE(state.is_terminal(win_player, utility));
    ASSERT_EQ(win_player, State::PLAYER_AI);
    ASSERT_EQ(utility, std::numeric_limits<float>::infinity());
}

} // namespace game
} // namespace ai

