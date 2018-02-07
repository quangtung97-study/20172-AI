#ifndef AI_GAME_MINIMAX_HPP
#define AI_GAME_MINIMAX_HPP

#include <limits>

namespace ai {
namespace game {

template <typename State>
class Minimax {
private:
    State state_;
    using ActionType = typename State::ActionType;
    using PlayerType = typename State::PlayerType;

    struct MoveGaurd {
        State& state_;
        ActionType action_;

        MoveGaurd(State& state, ActionType action)
            : state_{state}, action_{action} { state_.move(action_); }

        ~MoveGaurd() { state_.unmove(action_); }
    };
    
    ActionType final_action_;

private:
    float minimax(int depth) {
        auto player = state_.next_player();
        PlayerType win_player;
        float utility;

        if (state_.is_terminal(win_player, utility))
            return utility;

        if (player == State::PLAYER_AI) {
            float max = -std::numeric_limits<float>::infinity();
            for (auto action: state_.legalActions()) {
                MoveGaurd gaurd{state_, action};
                float minimax_result = minimax(depth + 1);
                if (max < minimax_result) {
                    max = minimax_result;
                    if (depth == 1) {
                        final_action_ = action;
                    }
                }
            }
            return max;
        }
        
        if (player == State::PLAYER_PERSON) {
            float min = std::numeric_limits<float>::infinity();
            for (auto action: state_.legalActions()) {
                MoveGaurd gaurd{state_, action};
                float minimax_result = minimax(depth + 1);
                if (min > minimax_result) {
                    min = minimax_result;
                }
            }
            return min;
        }
    }

public:
    Minimax(const State& state): state_{state} {}

    ActionType next_action() {
        minimax(1);
        return final_action_;
    }

}; // class Minimax

} // namespace game
} // namespace ai

#endif // AI_GAME_MINIMAX_HPP
