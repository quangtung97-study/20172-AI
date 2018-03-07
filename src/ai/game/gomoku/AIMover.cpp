#include <ai/game/gomoku/AIMover.hpp>
#include <SDL2/SDL_ttf.h>


namespace ai {
namespace game {
namespace gomoku {

AIMover::AIMover(State& state)
    : state_{state} {}

void AIMover::next_move_in_background() {
    moved_ = false;
    thinking_ = true;

    thread_ = std::thread([this]() {
        Action action = AI_next_move(state_);
        thinking_ = false;

        std::lock_guard<std::mutex> guard{mutex_};
        recent_move_ = action;
        moved_ = true;
    });
    thread_.detach();
}

bool AIMover::moved() {
    std::lock_guard<std::mutex> guard{mutex_};
    bool ret = moved_;
    moved_ = false;
    return ret;
}

Action AIMover::recent_move() const {
    std::lock_guard<std::mutex> guard{mutex_};
    return recent_move_;
}

bool AIMover::thinking() const {
    return thinking_;
}

} // namespace gomoku
} // namespace game
} // namespace ai

