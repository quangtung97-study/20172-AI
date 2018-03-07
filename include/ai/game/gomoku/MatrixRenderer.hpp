#ifndef AI_GAME_GOMOKU_MATRIXRENDERER_HPP
#define AI_GAME_GOMOKU_MATRIXRENDERER_HPP

#include <SDL2/SDL.h>
#include <vector>
#include "State.hpp"

namespace ai {
namespace game {
namespace gomoku {

typedef Action Position;

struct Coord {
    int x, y;
};

class MatrixRenderer {
private:
    std::vector<SDL_Texture *> matrix_;
    SDL_Renderer *renderer_ = nullptr;
    Coord slide_vector_ = {0, 0};
    Coord old_slide_vector_ = {0, 0};

    Coord pos_to_coord(Position pos) const;

public:
    const int matrix_size;
    const int screen_width;
    const int screen_height;
    const int cell_size = 30;
    SDL_Texture *N, *O, *X;

public:
    MatrixRenderer(
            SDL_Renderer *renderer,
            int matrix_size, 
            int screen_width, 
            int screen_height);

    MatrixRenderer(
            SDL_Renderer *renderer,
            int screen_width, 
            int screen_height)
        : MatrixRenderer(renderer, 100, screen_width, screen_height) {}

    ~MatrixRenderer();

    SDL_Texture *operator () (Position pos) const;

    SDL_Texture *& operator () (Position pos);

    void reset();

    void begin_slide();

    void slide(Coord dv);

    void render() const;

    Position coord_to_pos(Coord coord) const;

}; // class MatrixRenderer

} // namespace gomoku
} // namespace game
} // namespace ai

#endif // AI_GAME_GOMOKU_MATRIXRENDERER_HPP
