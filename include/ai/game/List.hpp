#ifndef AI_GAME_LIST_HPP
#define AI_GAME_LIST_HPP

namespace ai {
namespace game {

template <typename Item>
class List {
private:
public:
    class Iterator {
    private:
        Item num_ = 0;

    public:
        Iterator(Item num): num_{num} {}

        Iterator& operator ++ () {
            num_++;
            return *this;
        }

        Item& operator * () {
            return num_;
        }
        
        bool operator != (const Iterator& end) {
            return num_ != end.num_;
        }
    };

    Iterator begin() const {
        return Iterator(0);
    }

    Iterator end() const {
        return Iterator(10);
    }

}; // class List

} // namespace game
} // namespace ai

#endif // AI_GAME_LIST_HPP
