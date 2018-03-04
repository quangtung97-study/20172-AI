#include <ai/game/gomoku/MoveOrderer.hpp>
#include <algorithm>

namespace ai {
namespace game {
namespace gomoku {

std::size_t ActionsHash::operator() (const Actions& actions) const {
    std::size_t result = 0;
    int shift = 0;
    for (auto action: actions) {
        result = result ^ (action.x << shift) ^ (action.y << (shift + 1));
        shift += 2;
    }
    return result;
}

bool MoveOrderer::actions_stored() const {
    auto it = actions_map_.find(move_stack_);
    return it != actions_map_.end();
}

void MoveOrderer::sort() {
}

void MoveOrderer::add_action(Action action, float hvalue) {
    actions_map_[move_stack_].push_back({action, hvalue});
}

std::vector<Action> MoveOrderer::sorted_legal_actions() const {
    std::vector<Action> result;
    for (auto& e: actions_map_.at(move_stack_)) 
        result.push_back(e.action);
    return result;
}

} // namespace gomoku
} // namespace game
} // namespace ai

