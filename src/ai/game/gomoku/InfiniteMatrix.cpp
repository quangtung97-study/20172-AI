#include <ai/game/gomoku/InfiniteMatrix.hpp>

namespace ai {
namespace game {
namespace gomoku {

int index_of(int x, int y, Frame frame) {
    return x - frame.x + (y - frame.y) * frame.w;
}

int fall_inside(int x, int y, Frame frame) {
    int dx = x - frame.x;
    int dy = y - frame.y;
    return (0 <= dx && dx < frame.w) && (0 <= dy && dy < frame.h);
}

int align_size(int old, int d) {
    if (d >= 0)
        return ((d + old) / old) * old;
    else
        return ((-d - 1 + old * 2) / old) * old;
}

int align_distance(int old, int d) {
    return ((d + 1 - old) / old) * old;
}

} // namespace gomoku
} // namespace game
} // namespace ai

