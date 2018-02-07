#ifndef AI_GAME_TICTACTOE_STATE_HPP
#define AI_GAME_TICTACTOE_STATE_HPP

#include <array>
#include <vector>
#include <algorithm>
#include <limits>

namespace ai {
namespace game {
namespace tictactoe {

class Action {
private:
    int x_, y_;

public:
    Action(int x, int y): x_{x}, y_{y} {}

    Action(): Action(0, 0) {}

    Action(const Action& other) {
        x_ = other.x_;
        y_ = other.y_;
    }

    Action& operator = (const Action& other) {
        x_ = other.x_;
        y_ = other.y_;
        return *this;
    }

    int x() const { return x_; }

    int y() const { return y_; }
};

class State {
public:
    enum Player {
        PLAYER_PERSON,
        PLAYER_AI,
        PLAYER_NONE
    };
    
    enum CellValue: unsigned char {
        NONE,
        X, 
        O
    };

    using ActionType = Action;
    using PlayerType = Player;

private:
    const Player first_player_;
    Player next_player_;

    void switch_player() {
        switch (next_player()) {
            case PLAYER_PERSON:
                next_player_ = PLAYER_AI;
                break;
            case PLAYER_AI:
                next_player_ = PLAYER_PERSON;
                break;
            default:
                break;
        }
    }


    CellValue value_of(Player player) {
        return player == first_player_ ? X : O;
    }

#define MATRIX_SIZE 3

    std::array<CellValue, MATRIX_SIZE * MATRIX_SIZE> cells_;

    int x_coordinate_of(int index) const {
        return index % MATRIX_SIZE + 1;
    }

    int y_coordinate_of(int index) const {
        return index / MATRIX_SIZE + 1;
    }

public:
    State(Player first_player)
        : first_player_{first_player}, next_player_{first_player} 
    {
        std::fill(cells_.begin(), cells_.end(), NONE);    
    }

    State(std::initializer_list<CellValue> list, Player first_player)
        : first_player_{first_player}, next_player_{first_player} 
    {
        auto it = std::copy(list.begin(), list.end(), cells_.begin());
        std::fill(it, cells_.end(), NONE);
    }

    State(const State& other): first_player_{other.first_player_} {
        next_player_ = other.next_player_;
        cells_ = other.cells_;
    }

    Player next_player() const { return next_player_; }

    auto operator () (int x, int y) const {
        x--; y--;
        return cells_[y * MATRIX_SIZE + x];
    }

    auto& operator () (int x, int y) {
        x--; y--;
        return cells_[y * MATRIX_SIZE + x];
    }

    void move(Action action) {
        (*this)(action.x(), action.y()) = value_of(next_player_);
        switch_player();
    }

    void unmove(Action action) {
        (*this)(action.x(), action.y()) = NONE;
        switch_player();
    }

    auto legalActions() const {
        std::vector<Action> actions;
        actions.reserve(9);
        for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
            if (cells_[i] == NONE)
                actions.push_back(Action{x_coordinate_of(i), y_coordinate_of(i)});
        }
        return actions;
    }

private:
    template <typename Array>
    void fill_zero(Array& array) const { 
        std::fill(array.begin(), array.end(), 0);
    }

    bool on_first_diagonal(int index) const {
        return index % 4 == 0;
    }
    
    bool on_second_diagonal(int index) const {
        switch (index) {
            case 2: case 4: case 6:
                return true;
            default:
                return false;
        }
    }

    float value_of_utility(Player player) const {
        const float inf = std::numeric_limits<float>::infinity();
        return player == PLAYER_AI ? inf : -inf;
    }

    Player next_player_of(Player player) const {
        switch (player) {
            case PLAYER_PERSON: 
                return PLAYER_AI;
            case PLAYER_AI:
                return PLAYER_PERSON;
            default:
                return PLAYER_NONE;
        }
    }

    friend class State_on_diagonal_Test;

public:
    bool is_terminal(Player& win_player, float& utility) const {
        const int row_offset = 0;
        const int col_offset = 3;
        const int diagonal_offset = 6;

        std::array<unsigned char, 8> x_line_count;
        fill_zero(x_line_count);

        std::array<unsigned char, 8> o_line_count;
        fill_zero(o_line_count);

        int non_none_count = 0;

        for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
            if (cells_[i] == NONE)
                continue;

            non_none_count++;
            if (cells_[i] == X) {
                x_line_count[row_offset + y_coordinate_of(i) - 1]++;
                x_line_count[col_offset + x_coordinate_of(i) - 1]++;
                if (on_first_diagonal(i))
                    x_line_count[diagonal_offset]++;
                if (on_second_diagonal(i))
                    x_line_count[diagonal_offset + 1]++;
            }
            else {
                o_line_count[row_offset + y_coordinate_of(i) - 1]++;
                o_line_count[col_offset + x_coordinate_of(i) - 1]++;
                if (on_first_diagonal(i))
                    o_line_count[diagonal_offset]++;
                if (on_second_diagonal(i))
                    o_line_count[diagonal_offset + 1]++;
            }
        }

        if (std::any_of(x_line_count.begin(), x_line_count.end(),
                    [](auto count) -> bool { return count == 3; })) {
            win_player = first_player_;
            utility = value_of_utility(first_player_);
            return true;
        }

        if (std::any_of(o_line_count.begin(), o_line_count.end(),
                    [](auto count) -> bool { return count == 3; })) {
            win_player = next_player_of(first_player_);
            utility = value_of_utility(next_player_of(first_player_));
            return true;
        }

        if (non_none_count == 9) {
            win_player = PLAYER_NONE;
            utility = 0.0f;
            return true;
        }

        return false;
    }

}; // class State

} // namespace tictactoe
} // namespace game
} // namespace ai

#endif // AI_GAME_TICTACTOE_STATE_HPP
