#ifndef AI_GAME_PLAYER_HPP
#define AI_GAME_PLAYER_HPP

namespace ai {
namespace game {

class Player {
private:
    int id_;

public:
    Player(int id): id_{id} {}

    int id() const { return id_; }

}; // class Player

} // namespace game
} // namespace ai

#endif // AI_GAME_PLAYER_HPP
