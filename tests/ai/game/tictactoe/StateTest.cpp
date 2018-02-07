#include <gmock/gmock.h>
#include <ai/game/tictactoe/State.hpp>

namespace ai {
namespace game {
namespace tictactoe {

const auto N = State::NONE;
const auto X = State::X;
const auto O = State::O;

TEST(State, move_switch_player) {
    State state{State::PLAYER_PERSON};
    ASSERT_EQ(state.next_player(), State::PLAYER_PERSON);
    state.move(Action(1, 1));
    ASSERT_EQ(state.next_player(), State::PLAYER_AI);
    state.move(Action(1, 2));
    ASSERT_EQ(state.next_player(), State::PLAYER_PERSON);
}

TEST(State, function_call_operator) {
    const State state{State::PLAYER_PERSON};
    ASSERT_EQ(state(1, 1), N);
    ASSERT_EQ(state(3, 3), N);
}

TEST(State, move) {
    State state{State::PLAYER_PERSON};
    state.move(Action(2, 2));
    ASSERT_EQ(state(2, 2), X);
    state.move(Action(3, 2));
    ASSERT_EQ(state(3, 2), O);
}

TEST(State, unmove) {
    State state{State::PLAYER_PERSON};
    state.move(Action(2, 2));
    ASSERT_EQ(state(2, 2), X);
    state.move(Action(3, 2));
    ASSERT_EQ(state(3, 2), O);
    state.unmove(Action(3, 2));
    ASSERT_EQ(state(3, 2), N);
    ASSERT_EQ(state.next_player(), State::PLAYER_AI);
}

TEST(State, init_list) {
    State state{
        {N, X, O,
         X, O, N}, State::PLAYER_AI};
    ASSERT_EQ(state(3, 3), N);
}

void assert_action_equals(const Action& action, int x, int y) {
    ASSERT_EQ(action.x(), x);
    ASSERT_EQ(action.y(), y);
}

TEST(State, legalActions) {
    State state{
        {N, X, O,
         X, O, N}, State::PLAYER_AI};
    auto actions = state.legalActions();
    ASSERT_EQ(actions.size(), 5);
    assert_action_equals(actions[0], 1, 1);
    assert_action_equals(actions[1], 3, 2);
    assert_action_equals(actions[4], 3, 3);
}

TEST(State, on_diagonal) {
    State state{State::PLAYER_AI};
    // 0 1 2
    // 3 4 5
    // 6 7 8
    ASSERT_TRUE(state.on_first_diagonal(0));
    ASSERT_TRUE(state.on_first_diagonal(4));
    ASSERT_TRUE(state.on_first_diagonal(8));

    ASSERT_FALSE(state.on_first_diagonal(2));
    ASSERT_FALSE(state.on_first_diagonal(6));

    ASSERT_FALSE(state.on_first_diagonal(1));
    ASSERT_FALSE(state.on_first_diagonal(3));
    ASSERT_FALSE(state.on_first_diagonal(5));
    ASSERT_FALSE(state.on_first_diagonal(7));

    //--------------------------------------
    // 0 1 2
    // 3 4 5
    // 6 7 8
    ASSERT_FALSE(state.on_second_diagonal(0));
    ASSERT_FALSE(state.on_second_diagonal(8));

    ASSERT_TRUE(state.on_second_diagonal(2));
    ASSERT_TRUE(state.on_second_diagonal(4));
    ASSERT_TRUE(state.on_second_diagonal(6));

    ASSERT_FALSE(state.on_second_diagonal(1));
    ASSERT_FALSE(state.on_second_diagonal(3));
    ASSERT_FALSE(state.on_second_diagonal(5));
    ASSERT_FALSE(state.on_second_diagonal(7));
}

TEST(State, is_terminal) {
    State::Player win_player;
    float utility;

    State state1{
        {N, X, O,
         X, O, N,
         N, N, N}, State::PLAYER_AI};
    ASSERT_FALSE(state1.is_terminal(win_player, utility));

    //--------------------------------------
    State state2{
        {N, X, O,
         X, O, N,
         O, N, N}, State::PLAYER_AI};
    ASSERT_TRUE(state2.is_terminal(win_player, utility));
    ASSERT_EQ(win_player, State::PLAYER_PERSON);
    ASSERT_EQ(utility, -std::numeric_limits<float>::infinity());

    //--------------------------------------
    State state3{
        {O, X, X,
         X, X, O,
         O, X, O}, State::PLAYER_AI};
    ASSERT_TRUE(state3.is_terminal(win_player, utility));
    ASSERT_EQ(win_player, State::PLAYER_AI);
    ASSERT_EQ(utility, std::numeric_limits<float>::infinity());

    //--------------------------------------
    State state4{
        {O, X, X,
         X, O, O,
         O, X, N}, State::PLAYER_AI};
    ASSERT_FALSE(state4.is_terminal(win_player, utility));

    //--------------------------------------
    State state5{
        {O, O, X,
         X, O, O,
         O, X, X}, State::PLAYER_AI};
    ASSERT_TRUE(state5.is_terminal(win_player, utility));
    ASSERT_EQ(win_player, State::PLAYER_NONE);
    ASSERT_EQ(utility, 0.0f);
}

} // namespace tictactoe
} // namespace game
} // namespace ai

