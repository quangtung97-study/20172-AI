#ifndef AI_GAME_GOMOKU_HEURISTIC_HPP
#define AI_GAME_GOMOKU_HEURISTIC_HPP

#include <vector>
#include <bitset>

namespace ai {
namespace game {
namespace gomoku {

enum class Cell: char {
    NONE = 0,
    AI = 1, 
    HUMAN = -1
};

using Line = std::vector<Cell>;

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

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_HEURISTIC_HPP
