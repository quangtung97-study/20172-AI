#include <gmock/gmock.h>
#include <ai/search/imagepuzzle/Node.hpp>

namespace ai {
namespace search {
namespace imagepuzzle {

TEST(Node, constructor) {
    State state;
    Node node1(state);
    Node node2(&node1, Action::RIGHT);
    state.move(Action::RIGHT);

    ASSERT_TRUE(state == node2.state());
    ASSERT_EQ(node2.parent(), &node1);
    ASSERT_EQ(node1.parent(), nullptr);
    ASSERT_EQ(node2.action(), Action::RIGHT);
}

} // namespace imagepuzzle
} // namespace search
} // namespace ai

