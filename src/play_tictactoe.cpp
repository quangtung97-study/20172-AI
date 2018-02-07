#include <iostream>
#include <ai/game/Minimax.hpp>
#include <ai/game/tictactoe/State.hpp>

using ai::game::Minimax;
using namespace ai::game::tictactoe;

State::Player player_for(char input) {
    if (input == 'n' || input == 'N')
        return State::PLAYER_PERSON;
    return State::PLAYER_AI;
}

int main() {
    std::cout << "First player is AI (Y/n): ";
    std::string line;
    std::getline(std::cin, line);

    State state{player_for(line[0])};
    State::Player win_player;
    float utility;
    while (!state.is_terminal(win_player, utility)) {
        if (state.next_player() == State::PLAYER_AI) {
            Minimax<State> solver{state};
            auto next = solver.next_action();
            state.move(next);
            std::cout << "AI moved: " << next.x() << " " << next.y() << std::endl;
        }
        else {
            int x, y;
            std::cin >> x;
            std::cin >> y;
            state.move(Action(x, y));
        }
    }

    if (win_player == State::PLAYER_AI) {
        std::cout << "AI won!" << std::endl;
    }
    else if (win_player == State::PLAYER_PERSON) {
        std::cout << "Person won!" << std::endl;
    }
    else {
        std::cout << "Draw!" << std::endl;
    }
    
    return 0;
}
