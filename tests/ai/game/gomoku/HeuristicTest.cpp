#include <gmock/gmock.h>
#include <ai/game/gomoku/Heuristic.hpp>

namespace ai {
namespace game {
namespace gomoku {

const auto X = Cell::AI;
const auto N = Cell::NONE;
const auto O = Cell::HUMAN;

TEST(Heuristic, get_segment_bitset) {
    ASSERT_EQ(sizeof(Cell), 1);
    size_t cell_count;

    auto cells = get_segment_bitset({{X}, 0, 1}, X, cell_count);
    ASSERT_EQ(cells, 0b1);
    ASSERT_EQ(cell_count, 1);

    cells = get_segment_bitset({{X, X}, 0, 2}, X, cell_count);
    ASSERT_EQ(cells, 0b11);
    ASSERT_EQ(cell_count, 2);
    
    cells = get_segment_bitset({{X, N, X, X}, 0, 4}, X, cell_count);
    ASSERT_EQ(cells, 0b1101);
    ASSERT_EQ(cell_count, 4);

    cells = get_segment_bitset({{X, X, N, X, N, X}, 0, 6}, X, cell_count);
    ASSERT_EQ(cells, 0b1011);
    ASSERT_EQ(cell_count, 6);

    cells = get_segment_bitset({{X, X, N, X, N, X}, 1, 4}, X, cell_count);
    ASSERT_EQ(cells, 0b101);
    ASSERT_EQ(cell_count, 3);

    cells = get_segment_bitset({{O, O, N, O, N, O}, 1, 4}, O, cell_count);
    ASSERT_EQ(cells, 0b101);
    ASSERT_EQ(cell_count, 3);

    cells = get_segment_bitset({{O, O, N, O, N, O}, 1, 6}, O, cell_count);
    ASSERT_EQ(cells, 0b10101);
    ASSERT_EQ(cell_count, 5);

    cells = get_segment_bitset({{O, O, O, O, O, O}, 0, 6}, O, cell_count);
    ASSERT_EQ(cells, 0b11111);
    ASSERT_EQ(cell_count, 6);
}

LineView maximum_view(LineView line_view, size_t size);

TEST(Heuristic, maximum_view) {
    Line line{O, N, O, O, O, O, N};
    auto view = maximum_view({line, 0, 7}, 4);
    ASSERT_EQ(view.begin() - line.begin(), 2);
    ASSERT_EQ(view.end() - line.begin(), 6);
}

TEST(Heuristic, get_segment_infos) {
    Line line1{X, X, O, N, X, N, X, X};
    auto ai_segments = get_segment_infos(line1, X);
    ASSERT_EQ(ai_segments.size(), 2);
    ASSERT_EQ(ai_segments[0].cells, 0b11);
    ASSERT_EQ(ai_segments[1].cells, 0b1101);

    Line line2{O};
    ai_segments = get_segment_infos(line2, X);
    ASSERT_EQ(ai_segments.size(), 0);

    Line line3{O, N, N, O};
    ai_segments = get_segment_infos(line3, X);
    ASSERT_EQ(ai_segments.size(), 0);
}

TEST(Heuristic, get_segment_infos_distances) {
    Line line1{X, X, O, N, X, N, X, X};
    auto ai_segments = get_segment_infos(line1, X);
    ASSERT_EQ(ai_segments[0].distances[0], SegmentInfo::Infinity);
    ASSERT_EQ(ai_segments[0].distances[1], SegmentInfo::Zero);
    ASSERT_EQ(ai_segments[1].distances[0], SegmentInfo::One);
    ASSERT_EQ(ai_segments[1].distances[1], SegmentInfo::Infinity);

    Line line2{N, N, O, N, N, N, X, X, X, N, N};
    ai_segments = get_segment_infos(line2, X);
    ASSERT_EQ(ai_segments.size(), 1);
    ASSERT_EQ(ai_segments[0].cells, 0b111);
    ASSERT_EQ(ai_segments[0].distances[0], SegmentInfo::Infinity);
    ASSERT_EQ(ai_segments[0].distances[1], SegmentInfo::Infinity);

    Line line3{N, X, X, O, N, N, X, X, N, O};
    ai_segments = get_segment_infos(line3, X);
    ASSERT_EQ(ai_segments.size(), 2);
    ASSERT_EQ(ai_segments[0].cells, 0b11);
    ASSERT_EQ(ai_segments[1].cells, 0b11);
    ASSERT_EQ(ai_segments[1].distances[0], SegmentInfo::Infinity);
    ASSERT_EQ(ai_segments[1].distances[1], SegmentInfo::One);
}

TEST(Heuristic, cell_inverse_of) {
    ASSERT_EQ(inverse_of(Cell::AI), Cell::HUMAN);
    ASSERT_EQ(inverse_of(Cell::HUMAN), Cell::AI);
    ASSERT_EQ(inverse_of(Cell::NONE), Cell::NONE);
}

SegmentInfo::Distance left_distance_of(Line::const_iterator it, Line::const_iterator begin);

TEST(Heuristic, left_distance_of) {
    Line line1{X, X, X};
    ASSERT_EQ(left_distance_of(line1.begin(), line1.begin()), SegmentInfo::Infinity);
    Line line2{O, N, X, X};
    ASSERT_EQ(left_distance_of(line2.begin() + 2, line2.begin()), SegmentInfo::One);
    Line line3{O, X, X};
    ASSERT_EQ(left_distance_of(line3.begin() + 1, line3.begin()), SegmentInfo::Zero);
    Line line4{O, N, N, X, X};
    ASSERT_EQ(left_distance_of(line4.begin() + 3, line4.begin()), SegmentInfo::Infinity);
    Line line5{O, N, N, N, X, X};
    ASSERT_EQ(left_distance_of(line5.begin() + 4, line5.begin()), SegmentInfo::Infinity);
    Line line6{N, X, X};
    ASSERT_EQ(left_distance_of(line6.begin() + 1, line6.begin()), SegmentInfo::Infinity);
}

SegmentInfo::Distance right_distance_of(Line::const_iterator it, Line::const_iterator end);

TEST(Heuristic, right_distance_of) {
    Line line1{X, O};
    ASSERT_EQ(right_distance_of(line1.begin() + 1, line1.end()), SegmentInfo::Zero);
    Line line2{X, N, O};
    ASSERT_EQ(right_distance_of(line2.begin() + 1, line2.end()), SegmentInfo::One);
    Line line3{X, N, N, O};
    ASSERT_EQ(right_distance_of(line3.begin() + 1, line3.end()), SegmentInfo::Infinity);
    Line line4{X, N, N};
    ASSERT_EQ(right_distance_of(line4.begin() + 1, line4.end()), SegmentInfo::Infinity);
    Line line5{X}; ASSERT_EQ(right_distance_of(line5.begin() + 1, line5.end()), SegmentInfo::Infinity);
    Line line6{X, N};
    ASSERT_EQ(right_distance_of(line6.begin() + 1, line6.end()), SegmentInfo::Infinity);
}

Line::const_iterator reverse_search(Line::const_iterator it, Line::const_iterator begin, Cell value);

TEST(Heuristic, reverse_search) {
    Line line1{X, X, N, O};
    auto it = reverse_search(line1.end(), line1.begin(), X);
    ASSERT_EQ(it, std::next(line1.begin(), 2));

    Line line2{X, X, N, N, N, O};
    it = reverse_search(line1.end(), line1.begin(), X);
    ASSERT_EQ(it, std::next(line1.begin(), 2));
}

int index_in_weight_table_of(SegmentInfo segment);

TEST(Heuristic, segment_scoring_one) {
    SegmentInfo segment;
    segment.cells = 0b1;
    segment.cell_count = 1;

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_DOUBLE_EQ(score_of(segment), 1);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.75);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.75);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.5);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.5);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_DOUBLE_EQ(score_of(segment), 0);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.25);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.25);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_DOUBLE_EQ(score_of(segment), 0.5);
}

TEST(Heuristic, segment_scoring_4) {
    SegmentInfo segment;
    const float infinity = std::numeric_limits<float>::infinity();

    segment.cells = 0b1111;
    segment.cell_count = 4;

    const auto nan = std::numeric_limits<float>::quiet_NaN();

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_NE(score_of(segment), nan);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_NE(score_of(segment), nan);
}

TEST(Heuristic, segment_scoring_5) {
    SegmentInfo segment;
    const float infinity = std::numeric_limits<float>::infinity();

    segment.cells = 0b11111;
    segment.cell_count = 5;

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_FLOAT_EQ(score_of(segment), 0);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);
}

TEST(Heuristic, segment_scoring_6) {
    SegmentInfo segment;
    const float infinity = std::numeric_limits<float>::infinity();

    segment.cells = 0b11111;
    segment.cell_count = 6;

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Infinity;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Infinity;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::Zero;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::Zero;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);

    segment.distances[0] = SegmentInfo::One;
    segment.distances[1] = SegmentInfo::One;
    ASSERT_FLOAT_EQ(score_of(segment), infinity);
}

} // namespace gomoku
} // namespace game
} // namespace ai

