#include <gmock/gmock.h>
#include <ai/game/gomoku/State.hpp>
#include <algorithm>
#include <ai/game/gomoku/Heuristic.hpp>

namespace ai {
namespace game {
namespace gomoku {

static void assert_action_eq(Action value, Action compared) {
    ASSERT_EQ(value.x, compared.x);
    ASSERT_EQ(value.y, compared.y);
}

static void assert_contains(const std::vector<Action>& actions, Action compared) {
    auto it = std::find_if(actions.begin(), actions.end(), 
            [compared](auto action) {
                return action.x == compared.x && action.y == compared.y;
    });
    ASSERT_TRUE(it != actions.end());
}

static void assert_not_contains(const std::vector<Action>& actions, Action compared) {
    auto it = std::find_if(actions.begin(), actions.end(), 
            [compared](auto action) {
                return action.x == compared.x && action.y == compared.y;
    });
    ASSERT_TRUE(it == actions.end());
}

TEST(State, default_values) {
    ASSERT_EQ(Cell{}, Cell::NONE);
    ASSERT_EQ(bool{}, false);
}

TEST(State, accessor) {
    State state;
    state.move({0, 0});
    ASSERT_EQ(state(0, 0), Cell::HUMAN);
    state.move({1, 0});
    ASSERT_EQ(state(1, 0), Cell::AI);
    state.unmove();
    ASSERT_EQ(state(1, 0), Cell::NONE);
    state.move({-1, 0});
    ASSERT_EQ(state(-1, 0), Cell::AI);
}

TEST(State, legal_actions) {
    State state;
    const size_t width = allow_distance * 2 + 1;

    auto actions = state.legal_actions();
    ASSERT_EQ(actions.size(), 1);
    assert_action_eq(actions[0], {0, 0});

    state.move({0, 0});
    actions = state.legal_actions();
    ASSERT_EQ(actions.size(), width * width - 1);
    assert_contains(actions, {-1, 1});
    assert_contains(actions, {1, 1});
    assert_not_contains(actions, {0, 0});

    state.move({1, 0});
    actions = state.legal_actions();
    ASSERT_EQ(actions.size(), (width + 1) * width - 2);
    assert_not_contains(actions, {0, 0});
    assert_not_contains(actions, {1, 0});
    assert_contains(actions, {2, -1});
}

TEST(State, unmove) {
    const size_t width = allow_distance * 2 + 1;

    State state;
    state.move({0, 0});
    auto actions = state.legal_actions();
    ASSERT_EQ(actions.size(), width * width - 1);

    state.unmove();
    actions = state.legal_actions();
    ASSERT_EQ(actions.size(), 1);
}

const auto X = Cell::AI;
const auto N = Cell::NONE;
const auto O = Cell::HUMAN;

void assert_line_eq(const Line& a, const Line& b) {
    ASSERT_TRUE(std::equal(a.begin(), a.end(), b.begin(), b.end()));
}

void get_vertical_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action);

TEST(State, get_vertical_line) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(1, 0) = X;
    cells(3, 0) = X;
    Line line;
    get_vertical_line(line, cells, {0, 0});
    assert_line_eq(line, {X, X, N, X});
}

void get_horizontal_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action);

TEST(State, get_horizontal_line) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(0, 1) = X;
    cells(0, 4) = X;
    Line line;
    get_horizontal_line(line, cells, {0, 0});
    ASSERT_EQ(line.size(), 5);
    assert_line_eq(line, {X, X, N, N, X});
}

void get_first_diagonal_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action);

TEST(State, get_first_diagonal_line) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(1, 1) = X;
    cells(2, 2) = X;
    Line line;
    get_first_diagonal_line(line, cells, {1, 1});
    ASSERT_EQ(line.size(), 3);
    assert_line_eq(line, {X, X, X});
}

TEST(State, get_first_diagonal_line2) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(-1, 1) = X;
    cells(-3, 3) = X;

    Line line;
    get_first_diagonal_line(line, cells, {-1, 1});
    ASSERT_EQ(line.size(), 3);
    assert_line_eq(line, {N, X, N});
}

void get_second_diagonal_line(Line& line, const InfiniteMatrix<Cell>& cells, Action action);

TEST(State, get_second_diagonal_line) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(-1, 1) = X;
    cells(-3, 3) = X;
    Line line;
    get_second_diagonal_line(line, cells, {0, 0});
    ASSERT_EQ(line.size(), 4);
    assert_line_eq(line, {X, N, X, X});
}

float get_sum_lines_hvalue_at(
        const InfiniteMatrix<Cell>& cells_, 
        Action action, Cell current_player);

TEST(State, get_sum_lines_hvalue_at_vertical) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(1, 0) = X;
    cells(3, 0) = X;

    auto dh = get_sum_lines_hvalue_at(cells, {0, 0}, X);
    Line line1{X};
    Line line2{X, X, N, X};
    float compared = 3 * score_of_line(line1, X) + score_of_line(line2, X);
    ASSERT_DOUBLE_EQ(dh, compared);
}

TEST(State, get_sum_lines_hvalue_at_horizontal) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(0, 1) = X;
    cells(0, 2) = X;

    auto dh = get_sum_lines_hvalue_at(cells, {0, 0}, X);
    Line line1{X};
    Line line2{X, X, X};
    float compared = 3 * score_of_line(line1, X) + score_of_line(line2, X);
    ASSERT_DOUBLE_EQ(dh, compared);
}

TEST(State, get_sum_lines_hvalue_at_first_diagonal) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(1, 1) = X;
    cells(2, 2) = X;

    auto dh = get_sum_lines_hvalue_at(cells, {1, 1}, X);
    Line line1{X};
    Line line2{X, X, X};

    float compared = 3 * score_of_line(line1, X) + score_of_line(line2, X);
    ASSERT_DOUBLE_EQ(dh, compared);
}

TEST(State, get_sum_lines_hvalue_at_second_diagonal) {
    InfiniteMatrix<Cell> cells;
    cells(0, 0) = X;
    cells(-1, 1) = X;
    cells(-3, 3) = X;

    auto dh = get_sum_lines_hvalue_at(cells, {-1, 1}, X);
    Line line1{X};
    Line line2{X, N, X, X};

    float compared = 3 * score_of_line(line1, X) + score_of_line(line2, X);
    ASSERT_DOUBLE_EQ(dh, compared);
}

TEST(State, hvalue) {
    State state;
    state.move({0, 0});
    ASSERT_DOUBLE_EQ(state.hvalue(), -4);
    state.move({1, 0});
    ASSERT_DOUBLE_EQ(state.hvalue(), 0);
    state.unmove();
    ASSERT_DOUBLE_EQ(state.hvalue(), -4);
}

TEST(State, hvalue2) {
    InfiniteMatrix<Cell> cells;
    ASSERT_DOUBLE_EQ(get_sum_lines_hvalue_at(cells, {0, 0}, X), 0.0f);
    ASSERT_DOUBLE_EQ(get_sum_lines_hvalue_at(cells, {0, 0}, O), 0.0f);
    cells(0, 0) = O;
    ASSERT_DOUBLE_EQ(get_sum_lines_hvalue_at(cells, {0, 0}, X), 0.0f);
    ASSERT_DOUBLE_EQ(get_sum_lines_hvalue_at(cells, {0, 0}, O), 4.0f);
    cells(-1, -1) = X;
    ASSERT_DOUBLE_EQ(get_sum_lines_hvalue_at(cells, {-1, -1}, X), 3.5f);
    ASSERT_DOUBLE_EQ(get_sum_lines_hvalue_at(cells, {-1, -1}, O), 0.5f);
    State state;
    state.move({0, 0});
    ASSERT_DOUBLE_EQ(state.hvalue(), -4);
    state.move({-1, 1});
    ASSERT_DOUBLE_EQ(state.hvalue(), 0);
}

bool terminated_check(float new_ai_hvalue, float new_human_hvalue);

TEST(State, terminated_check) {
    const auto infinity = std::numeric_limits<float>::infinity();
    ASSERT_TRUE(terminated_check(infinity, 10));
    ASSERT_TRUE(terminated_check(100, infinity));
    ASSERT_FALSE(terminated_check(10, 10));
}

TEST(State, is_terminal) {
    State state; // HUMAN
    state.move({0, 0}); // AI
    state.move({0, 1}); // HUMAN
    state.move({1, 0}); // AI
    state.move({1, 1}); // HUMAN
    state.move({2, 0}); // AI
    state.move({2, 1}); // HUMAN
    state.move({3, 0}); // AI
    ASSERT_FALSE(state.is_terminal());
    state.move({3, 1}); // HUMAN
    ASSERT_FALSE(state.is_terminal());
    state.move({4, 0}); // AI
    ASSERT_TRUE(state.is_terminal());
    state.unmove(); // HUMAN
    ASSERT_FALSE(state.is_terminal());
    state.move({3, -1}); // AI
    ASSERT_EQ(state.current_player(), Cell::AI);
    ASSERT_FALSE(state.is_terminal());
    state.move({4, 1}); // HUMAN
    ASSERT_TRUE(state.is_terminal());
    ASSERT_EQ(state.current_player(), Cell::HUMAN);
}

TEST(State, maximizing) {
    State state(Cell::AI);
    ASSERT_TRUE(state.is_maximizing());
    state.move({0, 0});
    ASSERT_FALSE(state.is_maximizing());
}

} // namespace gomoku
} // namespace game
} // namespace ai

