#ifndef AI_GAME_GOMOKU_AIMOVER_HPP
#define AI_GAME_GOMOKU_AIMOVER_HPP

#include "State.hpp"
#include <thread>
#include <mutex>
#include <atomic>

namespace ai {
namespace game {
namespace gomoku {

class AIMover {
private:
    State& state_;
    std::thread thread_;
    bool moved_ = false;
    Action recent_move_;
    mutable std::mutex mutex_;
    std::atomic_bool thinking_{false};

public:
    AIMover(State& state);

    void next_move_in_background();

    bool moved();

    Action recent_move() const;

    bool thinking() const;

    ~AIMover();

}; // class AIMover

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_AIMOVER_HPP
