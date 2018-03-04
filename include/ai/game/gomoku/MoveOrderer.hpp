#ifndef AI_GAME_GOMOKU_MOVEORDERER_HPP
#define AI_GAME_GOMOKU_MOVEORDERER_HPP

#include "State.hpp"
#include <unordered_map>

namespace ai {
namespace game {
namespace gomoku {

typedef std::vector<Action> Actions;

struct ActionHvalue {
    Action action;
    float hvalue;
};

struct ActionsHash {
    std::size_t operator() (const Actions& actions) const;
};

class MoveOrderer {
private:
    std::unordered_map<Actions, std::vector<ActionHvalue>, ActionsHash> actions_map_;
    Actions move_stack_;

public:
    void add_action(Action action, float hvalue);

    bool actions_stored() const;

    void sort();

    void next(Action action) { move_stack_.push_back(action); }

    void prev() { move_stack_.pop_back(); }

    std::vector<Action> sorted_legal_actions() const;
}; // class MoveOrderer

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_MOVEORDERER_HPP
