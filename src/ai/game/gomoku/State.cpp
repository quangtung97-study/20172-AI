#include <ai/game/gomoku/State.hpp>
#include <ai/game/gomoku/Heuristic.hpp>
#include <algorithm>

namespace ai {
namespace game {
namespace gomoku {

State::State(Cell start_player): current_player_{start_player} {
    allow_cells_(0, 0) = 1;
}

std::vector<Action> State::legalActions() const {
    std::vector<Action> actions;
    actions.reserve(100);
    auto inused = allow_cells_.inused();
    for (int x = inused.x; x < inused.x + inused.w; x++)
        for (int y = inused.y; y < inused.y + inused.h; y++) {
            if (allow_cells_(x, y) != 0 && cells_(x, y) == Cell::NONE)
                actions.push_back({x, y});
        }
    return actions;
}

Cell State::operator() (int x, int y) const {
    return cells_(x, y);
}

static const int allow_distance = 2;

void State::move(Action action) {
    cells_(action.x, action.y) = current_player_;
    current_player_ = inverse_of(current_player_);

    for (int dx = -allow_distance; dx <= allow_distance; dx++)
        for (int dy = -allow_distance; dy <= allow_distance; dy++)
            allow_cells_(action.x + dx, action.y + dy)++;
}

void State::unmove(Action action) {
    cells_(action.x, action.y) = Cell::NONE;
    current_player_ = inverse_of(current_player_);

    for (int dx = -allow_distance; dx <= allow_distance; dx++)
        for (int dy = -allow_distance; dy <= allow_distance; dy++)
            allow_cells_(action.x + dx, action.y + dy)--;
}

void get_vertical_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action) {
    auto inused = cells.inused();
    line.clear();
    for (int x = inused.x, y = action.y; x < inused.w + inused.x; x++) {
        line.push_back(cells(x, y));
    }
}

void get_horizontal_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action) {
    auto inused = cells.inused();
    line.clear();
    for (int x = action.x, y = inused.y; y < inused.h + inused.y; y++) {
        line.push_back(cells(x, y));
    }
}

void get_first_diagonal_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action) {
    auto inused = cells.inused();
    line.clear();
    for (int d = -1; fall_inside(action.x + d, action.y + d, inused); d--) {
        line.push_back(cells(action.x + d, action.y + d));
    }
    std::reverse(line.begin(), line.end());
    for (int d = 0; fall_inside(action.x + d, action.y + d, inused); d++) {
        line.push_back(cells(action.x + d, action.y + d));
    }
}

void get_second_diagonal_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action)  {
    auto inused = cells.inused();
    line.clear();
    for (int d = -1; fall_inside(action.x + d, action.y - d, inused); d--) {
        line.push_back(cells(action.x + d, action.y - d));
    }
    std::reverse(line.begin(), line.end());
    for (int d = 0; fall_inside(action.x + d, action.y - d, inused); d++) {
        line.push_back(cells(action.x + d, action.y - d));
    }
}

float get_sum_lines_hvalue_at(
        const InfiniteMatrix<Cell>& cells, 
        Action action, Cell current_player)
{
    float result = 0.0f;
    static Line line(100);
    line.clear();

    get_vertical_line(line, cells, action);
    result += score_of_line(line, current_player);

    get_horizontal_line(line, cells, action);
    result += score_of_line(line, current_player);

    get_first_diagonal_line(line, cells, action);
    result += score_of_line(line, current_player);

    get_second_diagonal_line(line, cells, action);
    result += score_of_line(line, current_player);

    return result;
}

} // namespace gomoku
} // namespace game
} // namespace ai

