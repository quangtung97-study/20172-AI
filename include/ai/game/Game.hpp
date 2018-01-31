#ifndef AI_GAME_GAME_HPP
#define AI_GAME_GAME_HPP

namespace ai {
namespace game {

template <typename State, typename Action>
class Game {
private:
    State initial_;

public:
    Game(const State& initial): initial_{initial} {}

    State result(const State& state, const Action& ) const {
        return state;
    }

}; // class Game


} // namespace game
} // namespace ai

#endif // AI_GAME_GAME_HPP
