#include <gmock/gmock.h>
#include <ai/game/List.hpp>

namespace ai {
namespace game {

TEST(List, iterate) {
    List<int> list;
    int count = 0;
    for (auto& e: list) {
        count += e;
    }
    ASSERT_EQ(count, 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
}

} // namespace game
} // namespace ai

