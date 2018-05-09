#include <ai/game/gomoku/Heuristic.hpp>
#include <numeric>
#include <limits>
#include <algorithm>
#include <cassert>

namespace ai {
namespace game {
namespace gomoku {

Cell inverse_of(Cell cell) {
    return (Cell)(-(char)cell);
}

LineView maximum_view(LineView segment, size_t size) {
    int max = 0;
    LineView::iterator new_begin, new_end;
    for (auto it = segment.begin(); it != std::prev(segment.end(), size); ++it) {
        auto sum = std::accumulate(it, std::next(it, size), 0,
            [](int sum, Cell cell) { 
                return cell == Cell::NONE ? sum : sum + 1; 
        });

        if (sum > max) {
            max = sum;
            new_begin = it;
            new_end = std::next(it, size);
        }
    }
    return LineView{new_begin, new_end};
}

std::bitset<MAX_BIT_COUNT> get_segment_bitset(
        LineView segment, Cell compared_value, size_t& cell_count)
{
    std::bitset<MAX_BIT_COUNT> cells = 0;
    cell_count = segment.end() - segment.begin();
    if (cell_count > MAX_BIT_COUNT) {
        segment = maximum_view(segment, MAX_BIT_COUNT);
    }
    for (auto it = segment.begin(); it != segment.end(); ++it) 
        if (*it == compared_value)
            cells[std::distance(segment.begin(), it)] = 1;
    return cells;
}

SegmentInfo::Distance left_distance_of(Line::const_iterator it, Line::const_iterator begin) {
    Line::const_reverse_iterator rit{it};
    Line::const_reverse_iterator rend{begin};

    auto find_rit = std::find_if(rit, rend, [](Cell cell) { return cell != Cell::NONE; });

    if (find_rit == rend)
        return SegmentInfo::Infinity;
    if (std::distance(rit, find_rit) == 0)
        return SegmentInfo::Zero;
    if (std::distance(rit, find_rit) == 1)
        return SegmentInfo::One;
    return SegmentInfo::Infinity;
}

SegmentInfo::Distance right_distance_of(Line::const_iterator it, Line::const_iterator end) {
    auto find_it = std::find_if(it, end, [](Cell cell) { return cell != Cell::NONE; });

    if (find_it == end)
        return SegmentInfo::Infinity;
    if (std::distance(it, find_it) == 0)
        return SegmentInfo::Zero;
    if (std::distance(it, find_it) == 1)
        return SegmentInfo::One;
    return SegmentInfo::Infinity;
}

Line::const_iterator reverse_search(Line::const_iterator it, Line::const_iterator begin, Cell value) {
    Line::const_reverse_iterator rit{it};
    Line::const_reverse_iterator rend{begin};
    return std::find_if(rit, rend, [value](Cell cell) { return cell == value; }).base();
}

SegmentInfoList get_segment_infos(const Line& line, Cell compared_value) {
    SegmentInfoList result;

    auto it = line.begin();
    while (true) {
        it = std::find_if(it, line.end(), 
                [compared_value](Cell cell) { return cell == compared_value; });
        auto segment_begin = it;

        if (it == line.end())
            break;

        it = std::find_if(it, line.end(), 
                [compared_value](Cell cell) { 
                    return cell == inverse_of(compared_value); 
        });
        auto segment_end = reverse_search(it, segment_begin, compared_value);

        SegmentInfo info;
        size_t cell_count;
        info.cells = get_segment_bitset(
                LineView{segment_begin, segment_end}, 
                compared_value, cell_count);
        info.distances[0] = left_distance_of(segment_begin, line.begin());
        info.distances[1] = right_distance_of(segment_end, line.end());
        result.push_back(info);
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
            assert (false);
            return 0;
    }
}

float unscaling_score_of(std::bitset<MAX_BIT_COUNT> cells) {
    switch (cells.to_ulong()) {
        case 0b1:
            return 1;
        case 0b11:
            return 2.1;
        case 0b101:
            return 2.1;
        case 0b1001:
            return 1.5;

        case 0b111:
            return 4.3;
        case 0b1101:
        case 0b1011:
            return 4.3;
        case 0b11001:
        case 0b10011:
            return 8.7;
        case 0b10101:
            return 3.9;

        case 0b1111:
            return 17.5;
        case 0b11101:
        case 0b10111:
            return 17;
        case 0b11011:
            return 17;

        case 0b11111:
            return std::numeric_limits<float>::infinity();

        default:
            return 0;
    }
}

// allow block head and tail
float score_of(SegmentInfo segment) {
    const auto infinity = std::numeric_limits<float>::infinity();
    auto unscaling_score = unscaling_score_of(segment.cells);
    auto factor = scaling_factor_of(
            segment.distances[0], segment.distances[1]);
    if (unscaling_score == infinity) {
        if (segment.cell_count == 5 && factor == 0)
            return 0;
        return infinity;
    }
    return unscaling_score * factor;
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

