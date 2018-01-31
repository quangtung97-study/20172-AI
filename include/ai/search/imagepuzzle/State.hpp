#ifndef AI_SEARCH_IMAGEPUZZLE_STATE_HPP
#define AI_SEARCH_IMAGEPUZZLE_STATE_HPP

#include <array>
#include <vector>
#include <algorithm>

namespace ai {
namespace search {
namespace imagepuzzle {

enum class Action {
    LEFT,
    RIGHT, 
    UP, 
    DOWN
};

#define IMAGE_SIZE 3

class State {
private:
    typedef unsigned char ValueType;
    std::array<ValueType, IMAGE_SIZE * IMAGE_SIZE> array_;
    int hole_index_;

public:
    State() {
        for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; i++) {
            array_[i] = i;
        }
        hole_index_ = 0;
    }

    State(std::initializer_list<ValueType> values) {
        std::copy(values.begin(), values.end(), array_.begin());
        auto it = std::find(array_.begin(), array_.end(), 0);
        hole_index_ = it - array_.begin();
    }

    ValueType operator() (int i, int j) const {
        i--; j--;
        return array_[i * IMAGE_SIZE + j];
    }

    std::vector<Action> legalActions() const {
        std::vector<Action> actions;
        int col = hole_index_ % IMAGE_SIZE + 1;
        int row = hole_index_ / IMAGE_SIZE + 1;
        if (col > 1) 
            actions.push_back(Action::LEFT);
        if (col < 3)
            actions.push_back(Action::RIGHT);
        if (row > 1)
            actions.push_back(Action::UP);
        if (row < 3)
            actions.push_back(Action::DOWN);
        return actions;
    }

    bool operator == (const State& other) {
        return std::equal(array_.begin(), array_.end(), 
                other.array_.begin(), other.array_.end());
    }

    bool operator != (const State& other) {
        return !std::equal(array_.begin(), array_.end(), 
                other.array_.begin(), other.array_.end());
    }

    void move(Action action) {
        int prev_hole_index = hole_index_;
        switch (action) {
            case Action::LEFT:
                hole_index_--;
                std::swap(array_[prev_hole_index], array_[hole_index_]);
                break;

            case Action::RIGHT:
                hole_index_++;
                std::swap(array_[prev_hole_index], array_[hole_index_]);
                break;

            case Action::UP:
                hole_index_ -= IMAGE_SIZE;
                std::swap(array_[prev_hole_index], array_[hole_index_]);
                break;

            case Action::DOWN:
                hole_index_ += IMAGE_SIZE;
                std::swap(array_[prev_hole_index], array_[hole_index_]);
                break;
        }
    }

}; // class State

} // namespace imagepuzzle
} // namespace search
} // namespace ai

#endif // AI_SEARCH_IMAGEPUZZLE_STATE_HPP
