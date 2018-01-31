#ifndef AI_SEARCH_IMAGEPUZZLE_PROBLEM_HPP
#define AI_SEARCH_IMAGEPUZZLE_PROBLEM_HPP

#include "Node.hpp"
#include <memory>

namespace ai {
namespace search {
namespace imagepuzzle {

class Problem {
public:
    using NodeType = Node;

    static std::unique_ptr<Node> child_of(Node *parent, Action action) {
        auto node = std::make_unique<Node>(parent, action);
        return node;
    }

}; // class Problem

} // namespace imagepuzzle
} // namespace search
} // namespace ai

#endif // AI_SEARCH_IMAGEPUZZLE_PROBLEM_HPP
