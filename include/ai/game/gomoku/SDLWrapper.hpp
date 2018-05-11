#ifndef AI_GAME_GOMOKU_SDLWRAPPER_HPP
#define AI_GAME_GOMOKU_SDLWRAPPER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "MatrixRenderer.hpp"
#include "AIMover.hpp"
#include <memory>
#include <string>
#include <optional>

namespace ai {
namespace game {
namespace gomoku {

class TextRenderer {
private:
    SDL_Texture *texture_;
    SDL_Renderer *renderer_;
    SDL_Rect rect_;

public:
    TextRenderer(TTF_Font *font, 
            SDL_Renderer *renderer,
            const std::string& string, 
            SDL_Color color,
            int x, int y);

    void render();

    ~TextRenderer();
};

class Game {
public:
    std::optional<Action> ai_prev_move_;
    bool going_to_next_game_ = false;
    bool you_won_displayed_ = false;
    bool ai_won_displayed_ = false;

    State state_;

    Game(Cell start_player): state_{start_player} {}
};

class SDLWrapper {
private:
    SDL_Window *window_;
    SDL_Renderer *renderer_;

    std::unique_ptr<MatrixRenderer> matrix_renderer_;
    std::unique_ptr<AIMover> ai_mover_;
    std::unique_ptr<TextRenderer> ai_thinking_;
    std::unique_ptr<TextRenderer> ai_won_;
    std::unique_ptr<TextRenderer> you_won_;
    std::unique_ptr<TextRenderer> guide_;

    Cell start_player_ = Cell::HUMAN;
    std::unique_ptr<Game> game_;

    bool may_drag_ = false;
    bool dragging_ = false;
    Coord begin_drag_ = {0, 0};

    TTF_Font *Sans_;

private:
    State& state() { return game_->state_; }

    void move(Position pos);

    void next_game();

    SDL_Texture *player_to_cell(Cell player) const;

    void handle_human_move(Position pos);

    bool handle_game_mouse_event(SDL_Event event);

    void init_texts();

    void display_texts();

public:
    SDLWrapper();

    void run();

    ~SDLWrapper();

}; // class SDLWrapper

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_SDLWRAPPER_HPP
