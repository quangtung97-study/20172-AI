#ifndef AI_SEARCH_DEPTHSEARCH_HPP
#define AI_SEARCH_DEPTHSEARCH_HPP

#include <queue>
#include <set>
#include <memory>

namespace ai {
namespace search {

template <typename Problem>
class BreadthSearch {
public:
    using NodeType = typename Problem::NodeType;
    using StateType = typename Problem::StateType;
    using ActionType = typename Problem::ActionType;
    using ResultType = typename Problem::ResultType;

private:
    const Problem& problem_;
    
public:
    BreadthSearch(const Problem& problem): problem_{problem} {}

    ResultType solve() {
        NodeType root{problem_.initial_state()};
        if (problem_.is_terminal(&root))
            return problem_.solution(&root);

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
                if (explored.find(child->state()) != explored.end())
                    continue;

                if (problem_.is_terminal(child.get()))
                    return problem_.solution(child.get());

                frontier.push(child.get());
                explored.insert(child->state());
                nodes.push_back(std::move(child));
            }
        }
    }

}; // class BreadthSearch

} // namespace search
} // namespace ai

#endif // AI_SEARCH_DEPTHSEARCH_HPP
