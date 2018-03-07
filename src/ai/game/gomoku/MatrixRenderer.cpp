#include <ai/game/gomoku/MatrixRenderer.hpp>
#include <cmath>
#include <SDL2/SDL_image.h>

namespace ai {
namespace game {
namespace gomoku {

MatrixRenderer::MatrixRenderer(
        SDL_Renderer *renderer,
        int matrix_size,
        int screen_width, 
        int screen_height) 
: 
    renderer_{renderer},
    matrix_size{matrix_size}, 
    screen_width{screen_width}, 
    screen_height{screen_height} 
{
    matrix_.resize(matrix_size * matrix_size);

    N = IMG_LoadTexture(renderer_, "assets/N.png");
    O = IMG_LoadTexture(renderer_, "assets/O.png");
    X = IMG_LoadTexture(renderer_, "assets/X.png");

    reset();
}

SDL_Texture *MatrixRenderer::operator () (Position pos) const {
    int i = matrix_size / 2 - pos.y - 1;
    int j = pos.x + matrix_size / 2;
    return matrix_[j + i * matrix_size];
}

SDL_Texture *& MatrixRenderer::operator () (Position pos) {
    int i = matrix_size / 2 - pos.y - 1;
    int j = pos.x + matrix_size / 2;
    return matrix_[j + i * matrix_size];
}

void MatrixRenderer::reset() {
    std::fill(matrix_.begin(), matrix_.end(), N);
}

void MatrixRenderer::begin_slide() {
    old_slide_vector_ = slide_vector_;
}

void MatrixRenderer::slide(Coord dv) {
    slide_vector_.x = old_slide_vector_.x + dv.x;
    slide_vector_.y = old_slide_vector_.y + dv.y;
}

void MatrixRenderer::render() const {
    for (int x = -matrix_size / 2; x < matrix_size / 2; x++)
        for (int y = -matrix_size / 2; y < matrix_size / 2; y++) {
            auto coord = pos_to_coord({x, y});
            SDL_Rect rect{coord.x, coord.y, cell_size, cell_size};
            SDL_RenderCopy(renderer_, (*this)(Position{x, y}), nullptr, &rect);
        }
}

Coord MatrixRenderer::pos_to_coord(Position pos) const {
    Coord coord;
    coord.x = screen_width / 2 + pos.x * cell_size + slide_vector_.x;
    coord.y = screen_height / 2 - cell_size 
        - pos.y * cell_size + slide_vector_.y;
    return coord;
}

Position MatrixRenderer::coord_to_pos(Coord coord) const {
    Position pos;
    pos.x = std::floor(float(coord.x - screen_width / 2 
                - slide_vector_.x) / cell_size);
    pos.y = std::floor(float(screen_height / 2 - 1 
                - coord.y + slide_vector_.y) / cell_size);
    return pos;
}

MatrixRenderer::~MatrixRenderer() {
    SDL_DestroyTexture(N);
    SDL_DestroyTexture(O);
    SDL_DestroyTexture(X);
}

} // namespace gomoku
} // namespace game
} // namespace ai

