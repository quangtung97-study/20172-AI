#include <iostream>
#include <string>
#include <ai/game/gomoku/State.hpp>
#include <algorithm>

using namespace ai::game::gomoku;
using std::cout;
using std::endl;

Cell player_for(char input) {
    if (input == 'n' || input == 'N')
        return Cell::HUMAN;
    return Cell::AI;
}

int main() {
    cout << "First player is AI (Y/n): ";
    std::string line;
    std::getline(std::cin, line);

    State state{player_for(line[0])};
    Cell win_player;
    while (!state.is_terminal()) {
        win_player = state.current_player();
        if (state.current_player() == Cell::AI) {
            Action action = AI_next_move(state);
            state.move(action);
            cout << "AI moved: " << action.x << " "
                << action.y << endl;
            // cout << "hvalue: " << state.hvalue() << endl;
        }
        else {
            int x, y;
            std::cin >> x;
            std::cin >> y;
            auto actions = state.legal_actions();
            auto it = std::find_if(actions.begin(), actions.end(),
                    [x, y](auto action) { return action.x == x && action.y == y; });
            if (it == actions.end()) {
                cout << "Illegal move!, move again!" << endl;
                continue;
            }
            state.move({x, y});
        }
    }

    if (win_player == Cell::AI)
        cout << "AI won!" << endl;
    else
        cout << "Human won!" << endl;
    return 0;
}
