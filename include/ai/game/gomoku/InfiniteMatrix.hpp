#ifndef AI_GAME_GOMOKU_INFINITEMATRIX_HPP
#define AI_GAME_GOMOKU_INFINITEMATRIX_HPP

#include <vector>

namespace ai {
namespace game {
namespace gomoku {

#define INIT_MATRIX_SIZE 51

struct Frame {
    int x = 0, y = 0;
    int w = 0, h = 0;
};

int index_of(int x, int y, Frame frame);
int fall_inside(int x, int y, Frame frame);
int align_size(int old, int d);
int align_distance(int old, int d);

template<typename Type>
std::vector<Type> frame_changed_copy(
        const std::vector<Type>& matrix, Frame old_frame, Frame new_frame)
{
    std::vector<Type> result;
    result.resize(new_frame.w * new_frame.h);
    for (int y = new_frame.y; y < new_frame.y + new_frame.h; y++) {
        for (int x = new_frame.x; x < new_frame.x + new_frame.w; x++) {
            if (fall_inside(x, y, old_frame)) 
                result[index_of(x, y, new_frame)] = matrix[index_of(x, y, old_frame)];
            else
                result[index_of(x, y, new_frame)] = Type{};
        }
    }
    return result;
}

Frame new_inused_frame(Frame old_inused, int x, int y);

void aligned_extend_range(int& begin, int& size, int new_value, bool& changed);

template <typename Type>
class InfiniteMatrix {
private:
    std::vector<Type> data_;
    Frame inused_;
    Frame frame_;

public:
    InfiniteMatrix(): InfiniteMatrix(INIT_MATRIX_SIZE) {}

    InfiniteMatrix(int init_matrix_size) {
        data_.resize(init_matrix_size * init_matrix_size);
        frame_.x = frame_.y = -init_matrix_size / 2;
        frame_.w = frame_.h = init_matrix_size;
    }

    Type operator () (int x, int y) const {
        return data_[index_of(x, y, frame_)];
    }

    Type& operator () (int x, int y) {
        bool changed = false;
        Frame new_frame = frame_;

        inused_ = new_inused_frame(inused_, x, y);

        aligned_extend_range(new_frame.x, new_frame.w, x, changed);
        aligned_extend_range(new_frame.y, new_frame.h, y, changed);

        if (changed) {
            data_ = frame_changed_copy(data_, frame_, new_frame);
            frame_ = new_frame;
        }

        return data_[index_of(x, y, frame_)];
    }

    auto frame() const { return frame_; }

    auto inused() const { return inused_; }

    auto size() const { return data_.size(); }
};
} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_INFINITEMATRIX_HPP
