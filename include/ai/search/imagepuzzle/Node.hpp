#ifndef AI_SEARCH_IMAGEPUZZLE_NODE_HPP
#define AI_SEARCH_IMAGEPUZZLE_NODE_HPP

#include "State.hpp"

namespace ai {
namespace search {
namespace imagepuzzle {

class Node {
private:
    State state_;
    const Node *parent_;
    Action action_;

public:
    using StateType = State;
    using ActionType = Action;

    Node(const State& state)
        : state_{state}, parent_{nullptr}, action_{Action::NONE} {}

    Node(const Node *parent, Action action)
        : state_{parent->state()}, parent_{parent}, action_{action} 
    {
        state_.move(action);
    }

    const State& state() const { return state_; }

    const Node *parent() const { return parent_; }

    Action action() const { return action_; }

}; // class Node

} // namespace imagepuzzle
} // namespace search
} // namespace ai

#endif // AI_SEARCH_IMAGEPUZZLE_NODE_HPP
