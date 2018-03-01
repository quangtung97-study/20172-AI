#include <ai/game/gomoku/State.hpp>

namespace ai {
namespace game {
namespace gomoku {

std::vector<Action> State::legalActions() const {
    std::vector<Action> actions;
    actions.push_back({0, 0});
    return actions;
}

} // namespace gomoku
} // namespace game
} // namespace ai

