#ifndef AI_GAME_GOMOKU_STATE_HPP
#define AI_GAME_GOMOKU_STATE_HPP

#include <vector>
#include <memory>
#include "Basic.hpp"

namespace ai {
namespace game {
namespace gomoku {

struct Action {
    int x, y;
};

class State {
private:

public:
    std::vector<Action> legalActions() const;

}; // class State

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_STATE_HPP
