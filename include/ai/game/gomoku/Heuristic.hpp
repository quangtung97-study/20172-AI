#ifndef AI_GAME_GOMOKU_HEURISTIC_HPP
#define AI_GAME_GOMOKU_HEURISTIC_HPP

#include <bitset>
#include "Basic.hpp"

namespace ai {
namespace game {
namespace gomoku {

class LineView {
private:
    Line::const_iterator begin_, end_;

public:
    using iterator = Line::const_iterator;

public:
    LineView(const Line& line, int begin, int end)
        : begin_{line.cbegin() + begin}, end_{line.cbegin() + end} {}

    LineView(Line::const_iterator begin, Line::const_iterator end)
        : begin_{begin}, end_{end} {}

    auto begin() const { return begin_; }

    auto end() const { return end_; }
};

#define MAX_BIT_COUNT 5

struct SegmentInfo {
    enum Distance {
        Infinity = 0,
        One = 1,
        Zero = 2
    } distances[2] = {Infinity, Infinity};

    std::bitset<MAX_BIT_COUNT> cells = 0;
};

std::bitset<MAX_BIT_COUNT> get_segment_bitset(LineView line_view, Cell compared_value);

typedef std::vector<SegmentInfo> SegmentInfoList;

SegmentInfoList get_segment_infos(const Line& line, Cell compared_value);

float score_of(SegmentInfo segment);

float score_of_line(Line line, Cell player);

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_HEURISTIC_HPP
