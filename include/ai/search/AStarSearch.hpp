#ifndef AI_SEARCH_ASTARSEARCH_HPP
#define AI_SEARCH_ASTARSEARCH_HPP

#include <vector>
#include <queue>
#include <memory>
#include <set>
#include <algorithm>

namespace ai {
namespace search {

template <typename Problem>
class AStarSearch {
private:
    Problem& problem_;
    using StateType = typename Problem::StateType;
    using NodeType = typename Problem::NodeType;

    std::set<StateType> explored_;
    std::vector<std::unique_ptr<NodeType>> nodes_;

    class NodeGreater {
    private:
        Problem& problem_;

    public:
        NodeGreater(Problem& problem): problem_{problem} {}

        bool operator () (const NodeType *first, const NodeType *second) const {
            return problem_.g(first) + problem_.h(first) 
                > problem_.g(second) + problem_.h(second);
        }
    };

    std::priority_queue<const NodeType *, 
        std::vector<const NodeType *>, 
        NodeGreater> heap_;

    void init_containers(const NodeType *root) {
        explored_.insert(root->state());
        nodes_.reserve(1000);
        heap_.push(root);
    }

    bool state_explored(const StateType& state) const {
        return explored_.find(state) != explored_.end();
    }

    void insert_new_node(std::unique_ptr<NodeType> node) {
        heap_.push(node.get());
        explored_.insert(node->state());
        nodes_.push_back(std::move(node));
    }

public:
    AStarSearch(Problem& problem): problem_{problem}, heap_{NodeGreater{problem_}} {}

    auto solve() {
        NodeType root{problem_.initial_state()};
        init_containers(&root);

        while (true) {
            if (heap_.empty())
                throw std::runtime_error("Can't solve with a star search");
            auto node = heap_.top();
            heap_.pop();

            if (problem_.is_terminal(node)) 
                return problem_.solution(node);

            for (auto action: node->state().legalActions()) {
                auto child = std::make_unique<NodeType>(node, action);
                if (state_explored(child->state()))
                    continue;

                insert_new_node(std::move(child));
            }
        }
    }

}; // class AStarSearch

} // namespace search
} // namespace ai

#endif // AI_SEARCH_ASTARSEARCH_HPP
