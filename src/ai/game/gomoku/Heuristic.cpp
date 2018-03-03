#include <ai/game/gomoku/Heuristic.hpp>
#include <numeric>
#include <limits>

namespace ai {
namespace game {
namespace gomoku {

Cell inverse_of(Cell cell) {
    return (Cell)(-(char)cell);
}

LineView maximum_view(LineView line_view, size_t size) {
    int max = 0;
    LineView::iterator new_begin, new_end;
    for (auto it = line_view.begin(); it != line_view.end() - size; ++it) {
        auto sum = std::accumulate(it, it + size, 0,
            [](int sum, Cell cell) { 
                return cell == Cell::NONE ? sum : sum + 1; 
        });

        if (sum > max) {
            max = sum;
            new_begin = it;
            new_end = it + size;
        }
    }
    return LineView{new_begin, new_end};
}

std::bitset<MAX_BIT_COUNT> get_segment_bitset(LineView line_view, Cell compared_value)
{
    std::bitset<MAX_BIT_COUNT> cells = 0;
    if (line_view.end() - line_view.begin() > MAX_BIT_COUNT) {
        line_view = maximum_view(line_view, MAX_BIT_COUNT);
    }
    for (int i = 0; i < line_view.end() - line_view.begin(); i++)
        if (*(line_view.begin() + i) == compared_value)
            cells[i] = 1;
    return cells;
}

SegmentInfo::Distance left_distance_of(Line::const_iterator it, Line::const_iterator begin) {
    if (it == begin)
        return SegmentInfo::Infinity;
    if (*(it - 1) != Cell::NONE) 
        return SegmentInfo::Zero;
    if (it - 1 == begin)
        return SegmentInfo::Infinity;
    if (*(it - 2) != Cell::NONE)
        return SegmentInfo::One;
    return SegmentInfo::Infinity;
}

SegmentInfo::Distance right_distance_of(Line::const_iterator it, Line::const_iterator end) {
    if (it + 1 == end)
        return SegmentInfo::Infinity;
    if (*(it + 1) != Cell::NONE)
        return SegmentInfo::Zero;
    if (it + 2 == end)
        return SegmentInfo::Infinity;
    if (*(it + 2) != Cell::NONE)
        return SegmentInfo::One;
    return SegmentInfo::Infinity;
}

Line::const_iterator reverse_search(Line::const_iterator it, Line::const_iterator begin, Cell value) {
    while (true) {
        if (*it == value)
            return it;
        if (it == begin)
            return begin;
        --it;
    }
}

SegmentInfoList get_segment_infos(const Line& line, Cell compared_value) {
    SegmentInfoList result;

    auto it = line.begin();
    while (true) {
        Line::const_iterator segment_begin = line.end();
        for (; it != line.end(); ++it) {
            if (*it == compared_value) {
                segment_begin = it;
                break;
            }
        }

        if (segment_begin == line.end())
            break;

        while (true) {
            ++it;
            if (it == line.end() || *it == inverse_of(compared_value)) {
                SegmentInfo info;
                auto segment_near_end = reverse_search(it - 1, line.begin(), compared_value);
                info.cells = get_segment_bitset(LineView{segment_begin, segment_near_end + 1}, compared_value);
                info.distances[0] = left_distance_of(segment_begin, line.begin());

                info.distances[1] = right_distance_of(segment_near_end, line.end());
                result.push_back(info);
                break;
            }
        }
    }

    return result;
}

float scaling_factor_of(SegmentInfo::Distance d1, SegmentInfo::Distance d2) {
    const auto Inf = SegmentInfo::Infinity;
    const auto One = SegmentInfo::One;
    const auto Zero = SegmentInfo::Zero;
    switch (d1 + d2 * 3) {
        case Inf + Inf * 3: 
            return 1;
        case One + Inf * 3:
        case Inf + One * 3:
            return 0.75;
        case Zero + Inf * 3:
        case Inf + Zero * 3:
            return 0.5;
        case Zero + One * 3:
        case One + Zero * 3:
            return 0.25;
        case Zero + Zero * 3:
            return 0;
        case One + One * 3:
            return 0.5;
        default:
            return 0;
    }
}

float unscaling_score_of(std::bitset<MAX_BIT_COUNT> cells) {
    switch (cells.to_ulong()) {
        case 0b1:
            return 1;
        case 0b11:
            return 5;
        case 0b101:
            return 5;
        case 0b1001:
            return 2.5;

        case 0b111:
            return 20;
        case 0b1101:
        case 0b1011:
            return 20;
        case 0b11001:
        case 0b10011:
            return 10;
        case 0b10101:
            return 5;

        case 0b1111:
            return 100;
        case 0b11101:
        case 0b10111:
            return 40;
        case 0b11011:
            return 40;

        case 0b11111:
            return std::numeric_limits<float>::infinity();

        default:
            return 0;
    }
}

float score_of(SegmentInfo segment) {
    return unscaling_score_of(segment.cells) 
        * scaling_factor_of(segment.distances[0], segment.distances[1]);
}

float score_of_line(Line line, Cell player) {
    float result = 0.0f;
    auto infos = get_segment_infos(line, player);
    for (auto info: infos)
        result += score_of(info);
    return result;
}

} // namespace gomoku
} // namespace game
} // namespace ai

