#ifndef AI_GAME_GOMOKU_BASIC_HPP
#define AI_GAME_GOMOKU_BASIC_HPP

#include <vector>

namespace ai {
namespace game {
namespace gomoku {

enum class Cell: char {
    NONE = 0,
    AI = 1, 
    HUMAN = -1
};

Cell inverse_of(Cell cell);

using Line = std::vector<Cell>;

} // namespace gomoku
} // namespace game
} // namespace ai


#endif
