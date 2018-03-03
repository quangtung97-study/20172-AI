#include <gmock/gmock.h>
#include <ai/game/gomoku/InfiniteMatrix.hpp>

namespace ai {
namespace game {
namespace gomoku {

void assert_frame_eq(Frame value, Frame compared) {
    ASSERT_EQ(value.x, compared.x);
    ASSERT_EQ(value.y, compared.y);
    ASSERT_EQ(value.w, compared.w);
    ASSERT_EQ(value.h, compared.h);
}

TEST(Frame, trivial_copy) {
    Frame frame;
    frame.x = 10;
    frame.y = 20;
    frame.w = 30;
    frame.h = 40;
    Frame copy = frame;
    assert_frame_eq(copy, frame);
}

TEST(InfiniteMatrix, assert_frame_eq) {
    Frame frame;
    frame.x = 1;
    frame.y = 2;
    frame.w = 3;
    frame.h = 4;
    assert_frame_eq(frame, {1, 2, 3, 4});
}

TEST(InfiniteMatrix, accessor) {
    InfiniteMatrix<int> matrix;

    ASSERT_EQ(INIT_MATRIX_SIZE, 51);
    ASSERT_EQ(matrix.size(), 51 * 51);
    auto frame = matrix.frame();
    assert_frame_eq(frame, {-25, -25, 51, 51});

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
    assert_frame_eq(matrix.frame(), {0, 0, 1, 1});
    
    matrix(0, 0) = 12;
    matrix(1, 0) = 13;

    ASSERT_EQ(matrix.size(), 2);
    assert_frame_eq(matrix.frame(), {0, 0, 2, 1});

    matrix(0, -1) = 14;
    ASSERT_EQ(matrix.size(), 2 * 2);
    assert_frame_eq(matrix.frame(), {0, -1, 2, 2});

    ASSERT_EQ(matrix(0, 0), 12);
    ASSERT_EQ(matrix(1, 0), 13);
    ASSERT_EQ(matrix(0, -1), 14);
}

TEST(InfiniteMatrix, resize2) {
    InfiniteMatrix<int> matrix(1);
    matrix(0, 0) = 10;
    matrix(1, 1) = 25;
    ASSERT_EQ(matrix.size(), 4);
    matrix(0, 0) = 12;
    ASSERT_EQ(matrix.size(), 4);
    matrix(-1, -1) = 14;
    ASSERT_EQ(matrix.size(), 16);

    ASSERT_EQ(matrix(1, 0), 0);
    ASSERT_EQ(matrix(0, 0), 12);
    ASSERT_EQ(matrix(1, 1), 25);
    ASSERT_EQ(matrix(-1, -1), 14);
}

TEST(InfiniteMatrix, new_inused_frame) {
    Frame old_inused{0, 0, 0, 0};
    auto new_inused = new_inused_frame(old_inused, 0, 0);
    assert_frame_eq(new_inused, {0, 0, 1, 1});
    new_inused = new_inused_frame(new_inused, -3, 4);
    assert_frame_eq(new_inused, {-3, 0, 4, 5});
}

TEST(InfiniteMatrix, new_inused_frame2) {
    Frame old_inused{0, 0, 0, 0};
    auto new_inused = new_inused_frame(old_inused, -3, 4);
    assert_frame_eq(new_inused, {-3, 4, 1, 1});
}

TEST(InfiniteMatrix, inused) {
    InfiniteMatrix<int> matrix(5);
    assert_frame_eq(matrix.inused(), {0, 0, 0, 0});
    assert_frame_eq(matrix.frame(), {-2, -2, 5, 5});
    matrix(1, 2);
    assert_frame_eq(matrix.inused(), {1, 2, 1, 1});
}

} // namespace gomoku
} // namespace game
} // namespace ai

