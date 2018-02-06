#ifndef AI_SEARCH_DEPTHSEARCH_HPP
#define AI_SEARCH_DEPTHSEARCH_HPP

#include <vector>
#include <memory>
#include <set>

namespace ai {
namespace search {

template <typename Problem>
class DepthSearch {
private:
    using NodeType = typename Problem::NodeType;
    using StateType = typename Problem::StateType;

    const Problem& problem_;
    std::set<StateType> explored_;
    std::vector<std::unique_ptr<NodeType>> nodes_;
    const NodeType *final_node;
    bool solved_ = false;

    bool state_explored(const StateType& state) {
        return explored_.find(state) != explored_.end();
    }

    void insert_new_node(std::unique_ptr<NodeType> node) {
            explored_.insert(node->state());
            nodes_.push_back(std::move(node));
    }

public:
    DepthSearch(const Problem& problem): problem_{problem} {}

    void recursion_step(const NodeType *node) {
        if (problem_.is_terminal(node)) {
            final_node = node;
            solved_ = true;
            return;
        }

        for (auto action: node->state().legalActions()) {
            auto child = std::make_unique<NodeType>(node, action);

            if (state_explored(child->state()))
                continue;

            const NodeType *childp = child.get();
            insert_new_node(std::move(child));

            recursion_step(childp);

            if (solved_)
                return;
        }
    }

    auto solve() {
        NodeType root{problem_.initial_state()};

        explored_.insert(root.state());
        nodes_.reserve(1000);

        recursion_step(&root);
        return problem_.solution(final_node);
    }

}; // class DepthSearch

} // namespace search
} // namespace ai

#endif // AI_SEARCH_DEPTHSEARCH_HPP
