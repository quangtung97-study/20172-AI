#include <gmock/gmock.h>
#include <ai/game/gomoku/MoveOrderer.hpp>

namespace ai {
namespace game {
namespace gomoku {

static void assert_action_eq(Action value, Action compared) {
    ASSERT_EQ(value.x, compared.x);
    ASSERT_EQ(value.y, compared.y);
}

TEST(MoveOrderer, actions_equals) {
    Actions actions1{{1, 2}, {3, 4}};
    Actions actions2{{1, 2}, {5, 6}};
    Actions actions3{{1, 7}, {3, 4}};

    ASSERT_TRUE(actions1 == actions1);
    ASSERT_FALSE(actions1 == actions2);
    ASSERT_FALSE(actions1 == actions3);
}

TEST(MoveOrderer, actions_stored) {
    MoveOrderer orderer;
    ASSERT_FALSE(orderer.actions_stored());

    orderer.add_action({2, -3}, 20.0f);
    orderer.add_action({3, -1}, 10.0f);
    ASSERT_TRUE(orderer.actions_stored());

    auto actions = orderer.sorted_legal_actions();
    ASSERT_EQ(actions.size(), 2);
    assert_action_eq(actions[0], {3, -1});
    assert_action_eq(actions[1], {2, -3});

    orderer.next({2, -3});
    ASSERT_FALSE(orderer.actions_stored());
}

TEST(MoveOrderer, ActionsHash) {
    Actions actions{{1, 2}, {4, 3}, {5, 6}};
    ActionsHash hash;
    ASSERT_EQ(hash(actions), 1 ^ (2 << 1) ^ (4 << 2) ^ 
            (3 << 3) ^ (5 << 4) ^ (6 << 5));
}

} // namespace gomoku
} // namespace game
} // namespace ai

