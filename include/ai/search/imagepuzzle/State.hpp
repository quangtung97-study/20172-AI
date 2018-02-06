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
    DOWN,
    NONE
};

#define IMAGE_SIZE 3

class State {
private:
    typedef unsigned char ValueType;
    std::array<ValueType, IMAGE_SIZE * IMAGE_SIZE> array_;
    int hole_index_;

    int x_coordinate_of(int index) const {
        return index / IMAGE_SIZE + 1;
    }

    int y_coordinate_of(int index) const {
        return index % IMAGE_SIZE + 1;
    }

    int index_of(ValueType value) const {
        return std::find(array_.begin(), array_.end(), value) - array_.begin();
    }

public:
    State() {
        for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; i++) {
            array_[i] = i;
        }
        hole_index_ = 0;
    }

    State(std::initializer_list<ValueType> values) {
        std::copy(values.begin(), values.end(), array_.begin());
        hole_index_ = index_of(0);
    }

    ValueType operator() (int i, int j) const {
        i--; j--;
        return array_[i * IMAGE_SIZE + j];
    }

    float manhattan_distance_to(const State& other) const {
        float distance = 0.0f;
        for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; i++) {
            if (array_[i] == 0)
                continue;

            int other_index = other.index_of(array_[i]);

            int y2 = y_coordinate_of(other_index);
            int y1 = y_coordinate_of(i);
            int x2 = x_coordinate_of(other_index);
            int x1 = x_coordinate_of(i);
            
            distance += std::abs(y2 - y1) + std::abs(x2 - x1);
        }
        return distance;
    }

    auto legalActions() const {
        std::vector<Action> actions;
        actions.reserve(4);
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

    bool operator == (const State& other) const {
        return std::equal(array_.begin(), array_.end(), 
                other.array_.begin(), other.array_.end());
    }

    bool operator != (const State& other) const {
        return !std::equal(array_.begin(), array_.end(), 
                other.array_.begin(), other.array_.end());
    }

    bool operator < (const State& other) const {
        for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; i++) {
            if (array_[i] < other.array_[i])
                return true;
            else if (array_[i] > other.array_[i])
                return false;
        }

        return false;
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

            default:
                break;
        }
    }

}; // class State

} // namespace imagepuzzle
} // namespace search
} // namespace ai

#endif // AI_SEARCH_IMAGEPUZZLE_STATE_HPP
