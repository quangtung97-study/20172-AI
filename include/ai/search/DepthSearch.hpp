#ifndef AI_SEARCH_DEPTHSEARCH_HPP
#define AI_SEARCH_DEPTHSEARCH_HPP

#include <vector>
#include <memory>
#include <set>

namespace ai {
namespace search {

template <typename Problem>
class DepthSearch {
public:
    using NodeType = typename Problem::NodeType;
    using StateType = typename Problem::StateType;
    using ActionType = typename Problem::ActionType;
    using ResultType = typename Problem::ResultType;

private:
    const Problem& problem_;
    std::set<StateType> explored_;
    std::vector<std::unique_ptr<NodeType>> nodes_;
    ResultType result_;
    bool solved_ = false;

public:
    DepthSearch(const Problem& problem): problem_{problem} {}

    void recursion_step(NodeType *node) {
        if (problem_.is_terminal(node)) {
            result_ = problem_.solution(node);
            solved_ = true;
            return;
        }

        for (auto action: node->state().legalActions()) {
            auto child = std::make_unique<NodeType>(node, action);
            if (explored_.find(child->state()) != explored_.end())
                continue;

            NodeType *childp = child.get();

            explored_.insert(childp->state());
            nodes_.push_back(std::move(child));

            recursion_step(childp);

            if (solved_)
                return;
        }
    }

    ResultType solve() {
        NodeType root{problem_.initial_state()};

        explored_.insert(root.state());
        nodes_.reserve(1000);

        recursion_step(&root);
        return result_;
    }

}; // class DepthSearch

} // namespace search
} // namespace ai

#endif // AI_SEARCH_DEPTHSEARCH_HPP
