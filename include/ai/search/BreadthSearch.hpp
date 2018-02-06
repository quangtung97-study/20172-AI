#ifndef AI_SEARCH_DEPTHSEARCH_HPP
#define AI_SEARCH_DEPTHSEARCH_HPP

#include <queue>
#include <set>
#include <memory>

namespace ai {
namespace search {

template <typename Problem>
class BreadthSearch {
private:
    using NodeType = typename Problem::NodeType;
    using StateType = typename Problem::StateType;

    const Problem& problem_;

    std::set<StateType> explored_;
    std::vector<std::unique_ptr<NodeType>> nodes_;
    std::queue<const NodeType *> frontier_;

    void init_containers(const NodeType *root) {
        explored_.insert(root->state());
        nodes_.reserve(1000);
        frontier_.push(root);
    }

    bool state_explored(const StateType& state) const {
        return explored_.find(state) != explored_.end();
    }

    void insert_new_node(std::unique_ptr<NodeType> node) {
        frontier_.push(node.get());
        explored_.insert(node->state());
        nodes_.push_back(std::move(node));
    }
    
public:
    BreadthSearch(const Problem& problem): problem_{problem} {}

    auto solve() {
        NodeType root{problem_.initial_state()};
        if (problem_.is_terminal(&root))
            return problem_.solution(&root);

        init_containers(&root);

        while (true) {
            if (frontier_.empty()) 
                throw std::runtime_error("Can't solve with breath-first search");

            auto node = frontier_.front(); 
            frontier_.pop();

            for (auto action: node->state().legalActions()) {
                auto child = std::make_unique<NodeType>(node, action);
                if (state_explored(child->state()))
                    continue;

                if (problem_.is_terminal(child.get()))
                    return problem_.solution(child.get());

                insert_new_node(std::move(child));
            }
        }
    }

}; // class BreadthSearch

} // namespace search
} // namespace ai

#endif // AI_SEARCH_DEPTHSEARCH_HPP
