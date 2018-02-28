#include <ai/game/gomoku/Heuristic.hpp>
#include <numeric>

namespace ai {
namespace game {
namespace gomoku {

Cell inverse_of(Cell cell) {
    if (cell == Cell::AI)
        return Cell::HUMAN;
    if (cell == Cell::HUMAN)
        return Cell::AI;
    return Cell::NONE;
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
    std::bitset<MAX_BIT_COUNT> cells;
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
                auto segment_near_end = reverse_search(it, line.begin(), compared_value);
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

} // namespace gomoku
} // namespace game
} // namespace ai

