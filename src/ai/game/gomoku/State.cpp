#include <ai/game/gomoku/State.hpp>
#include <ai/game/gomoku/Heuristic.hpp>
#include <algorithm>
#include <gsl/gsl>
#include <cassert>
#include <iostream>

namespace ai {
namespace game {
namespace gomoku {

bool operator == (Action action1, Action action2) {
    return action1.x == action2.x && action1.y == action2.y;
}

State::State(Cell start_player): current_player_{start_player} {
    allow_cells_(0, 0) = 1;
    terminated_stack_.push(false);
    hvalue_stack_.push(0.0f);
}

std::vector<Action> State::legal_actions() const {
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

float get_sum_lines_hvalue_at(
        const InfiniteMatrix<Cell>& cells, 
        Action action, Cell current_player);

bool terminated_check(float new_ai_hvalue, float new_human_hvalue) {
    const auto infinity = std::numeric_limits<float>::infinity();
    return new_ai_hvalue == infinity || new_human_hvalue == infinity;
}

void State::move(Action action) {
    move_stack_.push(action);

    auto& cell = cells_(action.x, action.y);
    cell = Cell::NONE;

    float old_ai_hvalue = get_sum_lines_hvalue_at(cells_, action, Cell::AI);
    float old_human_hvalue = get_sum_lines_hvalue_at(cells_, action, Cell::HUMAN);

    cell = current_player_;
    current_player_ = inverse_of(current_player_);

    for (int dx = -allow_distance; dx <= allow_distance; dx++)
        for (int dy = -allow_distance; dy <= allow_distance; dy++)
            allow_cells_(action.x + dx, action.y + dy)++;

    float new_ai_hvalue = get_sum_lines_hvalue_at(cells_, action, Cell::AI);
    float new_human_hvalue = get_sum_lines_hvalue_at(cells_, action, Cell::HUMAN);

    auto terminated = terminated_check(new_ai_hvalue, new_human_hvalue);
    terminated_stack_.push(terminated);

    float hvalue = hvalue_stack_.top();
    hvalue += (new_ai_hvalue - old_ai_hvalue) - (new_human_hvalue - old_human_hvalue);
    hvalue_stack_.push(hvalue);
}

void State::unmove() {
    auto action = move_stack_.top();
    move_stack_.pop();

    cells_(action.x, action.y) = Cell::NONE;
    current_player_ = inverse_of(current_player_);

    for (int dx = -allow_distance; dx <= allow_distance; dx++)
        for (int dy = -allow_distance; dy <= allow_distance; dy++)
            allow_cells_(action.x + dx, action.y + dy)--;

    terminated_stack_.pop();
    hvalue_stack_.pop();
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

float alphabeta(State& state, unsigned int depth, float alpha, float beta) {
    if (depth == 0 || state.is_terminal())
        return state.hvalue();
    if (state.is_maximizing()) {
        for (auto action: state.legal_actions()) {
            state.move(action);
            auto move_guard = gsl::finally([&state]() { state.unmove(); });

            float score = alphabeta(state, depth - 1, alpha, beta);
            alpha = std::max(alpha, score);

            if (beta <= alpha)
                break;
        }
        return alpha;
    }
    else {
        for (auto action: state.legal_actions()) {
            state.move(action);
            auto move_guard = gsl::finally([&state]() { state.unmove(); });

            float score = alphabeta(state, depth - 1, alpha, beta);
            beta = std::min(beta, score);

            if (beta <= alpha)
                break;
        }
        return beta;
    }
}

const unsigned int alphabeta_depth = 3;

Action AI_next_move(State& state) {
    assert (state.current_player() == Cell::AI);

    auto actions = state.legal_actions();
    Action result = actions[0];
    const auto infinity = std::numeric_limits<float>::infinity();
    float prev_hvalue = -infinity;
    for (auto action: state.legal_actions()) {
        state.move(action);
        auto move_guard = gsl::finally([&state]() { state.unmove(); });
        float new_hvalue = alphabeta(state, alphabeta_depth, -infinity, infinity);

        // std::cout << "---------------------------------------------" << std::endl;
        // std::cout << action.x << " " << action.y << std::endl;
        // std::cout << new_hvalue << std::endl;
        // std::cout << "---------------------------------------------" << std::endl;

        if (new_hvalue > prev_hvalue) {
            result = action;
            prev_hvalue = new_hvalue;
        }
    }
    return result;
}

} // namespace gomoku
} // namespace game
} // namespace ai

