#ifndef AI_SEARCH_IMAGEPUZZLE_NODE_HPP
#define AI_SEARCH_IMAGEPUZZLE_NODE_HPP

#include "State.hpp"
#include <limits>

namespace ai {
namespace search {
namespace imagepuzzle {

class Node {
private:
    State state_;
    const Node *parent_;
    Action action_;
    const float path_cost_;
    mutable float heuristic_value_ = std::numeric_limits<float>::max();

public:
    using StateType = State;
    using ActionType = Action;

    Node(const State& state)
        : state_{state}, parent_{nullptr}, action_{Action::NONE}, path_cost_{0} {}

    Node(const Node *parent, Action action)
        : state_{parent->state()}, parent_{parent}, action_{action}, 
        path_cost_{parent->path_cost_ + 1}
    {
        state_.move(action);
    }

    const State& state() const { return state_; }

    const Node *parent() const { return parent_; }

    float path_cost() const { return path_cost_; }

    void heuristic_value(float value) const { heuristic_value_ = value; }

    float heuristic_value() const { return heuristic_value_; }

    Action action() const { return action_; }

}; // class Node

} // namespace imagepuzzle
} // namespace search
} // namespace ai

#endif // AI_SEARCH_IMAGEPUZZLE_NODE_HPP
