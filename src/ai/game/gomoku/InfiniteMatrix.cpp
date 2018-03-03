#include <ai/game/gomoku/InfiniteMatrix.hpp>

namespace ai {
namespace game {
namespace gomoku {

int index_of(int x, int y, Frame frame) {
    return x - frame.x + (y - frame.y) * frame.w;
}

bool fall_inside(int x, int y, Frame frame) {
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

void aligned_extend_range(int& begin, int& size, int new_value, bool& changed) {
    int end = begin + size;
    int old_begin = begin;
    int old_size = size;

    if (new_value >= end) {
        changed = true;
        size = align_size(old_size, new_value - old_begin);
    }
    else if (new_value < old_begin) {
        changed = true;
        size = align_size(old_size, new_value - old_begin);
        begin = align_distance(old_size, new_value - old_begin) + old_begin;
    }
}

void extend_range(int& begin, int &size, int new_value) {
    if (size == 0) {
        size = 1;
        begin = new_value;
        return;
    }

    int end = begin + size;
    if (new_value < begin) 
        begin = new_value;
    else if (new_value >= end) 
        end = new_value + 1;
    size = end - begin;
}

Frame new_inused_frame(Frame frame, int x, int y) {
    extend_range(frame.x, frame.w, x);
    extend_range(frame.y, frame.h, y);
    return frame;
}

} // namespace gomoku
} // namespace game
} // namespace ai

