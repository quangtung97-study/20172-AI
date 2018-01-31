#ifndef AI_SEARCH_DEPTHSEARCH_HPP
#define AI_SEARCH_DEPTHSEARCH_HPP

#include <vector>
#include <queue>
#include <set>
#include <memory>
#include <algorithm>

namespace ai {
namespace search {

template <typename Problem>
class BreadthSearch {
public:
    using NodeType = typename Problem::NodeType;
    using StateType = typename NodeType::StateType;
    using ActionType = typename NodeType::ActionType;

private:
    const StateType begin_, end_;
    
public:
    BreadthSearch(const StateType& begin, const StateType& end)
        : begin_{begin}, end_{end} {}

    std::vector<ActionType> solution(NodeType *node) {
        std::vector<ActionType> actions;
        while (node->parent() != nullptr) {
            actions.push_back(node->action());
            node = node->parent();
        }
        std::reverse(actions.begin(), actions.end());
        return actions;
    }

    std::vector<ActionType> solve() {
        NodeType root{begin_};
        if (root.state() == end_)
            return solution(&root);

        std::vector<std::unique_ptr<NodeType>> nodes;
        nodes.reserve(1000);

        std::set<StateType> explored;
        explored.insert(root.state());

        std::queue<NodeType *> frontier;
        frontier.push(&root);

        while (true) {
            if (frontier.empty()) 
                throw std::runtime_error("Can't solve with breath-first search");

            NodeType *node = frontier.front(); frontier.pop();
            explored.insert(node->state());
            for (auto action: node->state().legalActions()) {
                auto child = std::make_unique<NodeType>(node, action);
                if (explored.find(child->state()) == explored.end()) {
                    if (child->state() == end_)
                        return solution(child.get());

                    frontier.push(child.get());
                    explored.insert(child->state());
                    nodes.push_back(std::move(child));
                }
            }
        }
    }

}; // class BreadthSearch

} // namespace search
} // namespace ai

#endif // AI_SEARCH_DEPTHSEARCH_HPP
