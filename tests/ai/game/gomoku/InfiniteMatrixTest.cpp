#include <gmock/gmock.h>
#include <ai/game/gomoku/InfiniteMatrix.hpp>

namespace ai {
namespace game {
namespace gomoku {

TEST(InfiniteMatrix, accessor) {
    InfiniteMatrix<int> matrix;

    ASSERT_EQ(INIT_MATRIX_SIZE, 51);
    ASSERT_EQ(matrix.size(), 51 * 51);
    auto frame = matrix.frame();
    ASSERT_EQ(frame.x, -25);
    ASSERT_EQ(frame.y, -25);
    ASSERT_EQ(frame.w, 51);
    ASSERT_EQ(frame.h, 51);

    matrix(0, 0) = 20;
    ASSERT_EQ(matrix(0, 0), 20);
}

TEST(InfiniteMatrix, align_distance_and_size) {
    ASSERT_EQ(align_size(10, 10), 20);
    ASSERT_EQ(align_size(10, 9), 10);
    ASSERT_EQ(align_size(10, 23), 30);
    ASSERT_EQ(align_size(10, 29), 30);
    ASSERT_EQ(align_size(10, -1), 20);
    ASSERT_EQ(align_size(10, -10), 20);
    ASSERT_EQ(align_size(10, -24), 40);

    ASSERT_EQ(align_distance(10, -3), -10);
    ASSERT_EQ(align_distance(10, -10), -10);
    ASSERT_EQ(align_distance(10, -23), -30);
}

TEST(InfiniteMatrix, frame_changed_copy) {
    std::vector<int> matrix{1, 2, 3, 4, 5, 6, 7, 8, 9};
    Frame old_frame{-1, -1, 3, 3};
    Frame new_frame{-4, -1, 6, 3};
    auto new_matrix = frame_changed_copy<>(matrix, old_frame, new_frame);
    ASSERT_EQ(new_matrix.size(), 6 * 3);
    ASSERT_EQ(new_matrix[0], 0);
    ASSERT_EQ(new_matrix[-1 + 4], 1);
    ASSERT_EQ(new_matrix[6 * 3 - 1], 9);
}

TEST(InfiniteMatrix, index_of) {
    ASSERT_EQ(index_of(0, 0, Frame{-1, -1, 3, 3}), 4);
    ASSERT_EQ(index_of(1, 1, Frame{-1, -1, 3, 3}), 8);
}

TEST(InfiniteMatrix, fall_inside) {
    ASSERT_TRUE(fall_inside(1, 1, Frame{-1, -1, 3, 3}));
    ASSERT_FALSE(fall_inside(2, 1, Frame{-1, -1, 3, 3}));
    ASSERT_FALSE(fall_inside(1, 2, Frame{-1, -1, 3, 3}));
    ASSERT_TRUE(fall_inside(-1, -1, Frame{-1, -1, 3, 3}));
    ASSERT_FALSE(fall_inside(-2, -1, Frame{-1, -1, 3, 3}));
}

TEST(InfiniteMatrix, resize) {
    InfiniteMatrix<int> matrix(1);

    ASSERT_EQ(matrix.size(), 1);
    auto frame = matrix.frame();
    ASSERT_EQ(frame.x, 0);
    ASSERT_EQ(frame.y, 0);
    ASSERT_EQ(frame.w, 1);
    ASSERT_EQ(frame.h, 1);
    
    matrix(0, 0) = 12;
    matrix(1, 0) = 13;

    ASSERT_EQ(matrix.size(), 2);
    frame = matrix.frame();
    ASSERT_EQ(frame.x, 0);
    ASSERT_EQ(frame.y, 0);
    ASSERT_EQ(frame.w, 2);
    ASSERT_EQ(frame.h, 1);

    matrix(0, -1) = 14;
    ASSERT_EQ(matrix.size(), 2 * 2);
    frame = matrix.frame();
    ASSERT_EQ(frame.x, 0);
    ASSERT_EQ(frame.y, -1);
    ASSERT_EQ(frame.w, 2);
    ASSERT_EQ(frame.h, 2);

    ASSERT_EQ(matrix(0, 0), 12);
    ASSERT_EQ(matrix(1, 0), 13);
    ASSERT_EQ(matrix(0, -1), 14);
}

} // namespace gomoku
} // namespace game
} // namespace ai

