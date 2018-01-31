#ifndef AI_GAME_STATE_HPP
#define AI_GAME_STATE_HPP

#include <ai/game/Player.hpp>
#include <ai/game/List.hpp>

namespace ai {
namespace game {

template <typename Player, typename Action>
class State {
private:
    const Player& current_;

public:
    State(const Player& current): current_{current} {}

    const Player& currentPlayer() const {
        return current_;
    }

    List<Action> legalActions() const {
        return List<Action>(*this);
    }

    bool is_terminal() const { return false; }

    float utility() const {
        return 0;
    }

}; // class State

} // namespace game
} // namespace ai

#endif // AI_GAME_STATE_HPP
