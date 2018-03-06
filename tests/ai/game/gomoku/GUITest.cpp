#include <gmock/gmock.h>
#include <ai/game/gomoku/State.hpp>
#include <algorithm>

namespace ai {
namespace game {
namespace gomoku {

#define WIDTH 600
#define HEIGHT 600
#define CELL_SIZE 30
#define MATRIX_SIZE 100

typedef Action Position;

struct Coord {
    int x, y;
};

static Coord pos_to_coord(Position pos, Coord trans={0, 0}) {
    Coord coord;
    coord.x = WIDTH / 2 + pos.x * CELL_SIZE + trans.x;
    coord.y = HEIGHT / 2 - CELL_SIZE 
        - pos.y * CELL_SIZE + trans.y;
    return coord;
}

static Position coord_to_pos(Coord coord, Coord trans={0, 0}) {
    Position pos;
    pos.x = std::floor(float(coord.x - WIDTH / 2 - trans.x) 
            / CELL_SIZE);
    pos.y = std::floor(float(HEIGHT / 2 - 1 - coord.y + trans.y) 
            / CELL_SIZE);
    return pos;
}

TEST(GUI, pos_to_coord) {
    ASSERT_EQ(std::floor(-3.0 / 2), -2);
    ASSERT_EQ(std::floor(float(29 - 300) / 30), -10);
}

TEST(GUI, coord_to_pos) {
    auto pos = coord_to_pos({0, 0});
    ASSERT_EQ(pos.x, -10);
    ASSERT_EQ(pos.y, 9);

    pos = coord_to_pos({29, 29});
    ASSERT_EQ(pos.x, -10);
    ASSERT_EQ(pos.y, 9);

    pos = coord_to_pos({30, 29});
    ASSERT_EQ(pos.x, -9);
    ASSERT_EQ(pos.y, 9);

    pos = coord_to_pos({0, 0}, {30, 0});
    ASSERT_EQ(pos.x, -11);
    ASSERT_EQ(pos.y, 9);

    pos = coord_to_pos({0, 0}, {0, 60});
    ASSERT_EQ(pos.x, -10);
    ASSERT_EQ(pos.y, 11);

    pos = coord_to_pos({0, 0}, {0, 59});
    ASSERT_EQ(pos.x, -10);
    ASSERT_EQ(pos.y, 11);
}

} // namespace gomoku
} // namespace game
} // namespace ai
