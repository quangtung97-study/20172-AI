#ifndef AI_GAME_GOMOKU_STATE_HPP
#define AI_GAME_GOMOKU_STATE_HPP

#include <vector>
#include <memory>
#include "Basic.hpp"
#include "InfiniteMatrix.hpp"

namespace ai {
namespace game {
namespace gomoku {

struct Action {
    int x, y;
};

class State {
private:
    InfiniteMatrix<Cell> cells_;
    InfiniteMatrix<unsigned char> allow_cells_;
    Cell current_player_;
    float heuristic_value_ = 0.0f;

public:
    State(Cell start_player = Cell::HUMAN);

    std::vector<Action> legalActions() const;

    Cell operator() (int x, int y) const;

    void move(Action action);

    void unmove(Action action);

    float heuristic_value() const { return heuristic_value_; }

}; // class State

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_STATE_HPP
