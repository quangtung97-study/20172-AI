#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <ai/game/gomoku/State.hpp>
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <cassert>

namespace ai {
namespace game {
namespace gomoku {

#define WIDTH 600
#define HEIGHT 600
#define CELL_SIZE 30
#define MATRIX_SIZE 100

static SDL_Texture *matrix[MATRIX_SIZE][MATRIX_SIZE];
static SDL_Texture *N, *O, *X;
static SDL_Renderer *renderer;
typedef Action Position;

struct Coord {
    int x, y;
};

static bool may_drag = false;
static bool dragging = false;
static Coord begin_drag = {0, 0};
static Coord slide_vector = {0, 0};
static Coord old_slide_vector;

static Cell start_player = Cell::HUMAN;
static State game_state;

static std::thread thread;
static std::atomic_bool ai_thinking{false};
static bool ai_moved = false;
static Action recent_ai_move;
static std::mutex ai_move_mutex;

TTF_Font *Sans;
SDL_Rect ai_thinking_msg_rect;
SDL_Texture *ai_thinking_msg;

static void reset_matrix() {
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            matrix[i][j] = N;
}

static void set_matrix(Position pos, SDL_Texture *value) {
    int i = MATRIX_SIZE / 2 - pos.y - 1;
    int j = pos.x + MATRIX_SIZE / 2;
    matrix[i][j] = value;
}

static SDL_Texture *get_matrix(Position pos) {
    int i = MATRIX_SIZE / 2 - pos.y - 1;
    int j = pos.x + MATRIX_SIZE / 2;
    return matrix[i][j];
}

static Coord pos_to_coord(Position pos, Coord trans={0, 0}) {
    Coord coord;
    coord.x = WIDTH / 2 + pos.x * CELL_SIZE + trans.x;
    coord.y = HEIGHT / 2 - CELL_SIZE 
        - pos.y * CELL_SIZE + trans.y;
    return coord;
}

static Position coord_to_pos(Coord coord, Coord trans={0, 0}) {
    Position pos;
    pos.x = std::floor(float(coord.x - WIDTH / 2 - trans.x) 
            / CELL_SIZE);
    pos.y = std::floor(float(HEIGHT / 2 - 1 - coord.y + trans.y) 
            / CELL_SIZE);
    return pos;
}

static void display_matrix() {
    for (int x = -MATRIX_SIZE / 2; x < MATRIX_SIZE / 2; x++)
        for (int y = -MATRIX_SIZE / 2; 
                y < MATRIX_SIZE / 2; y++) {
            auto coord = pos_to_coord({x, y}, slide_vector);
            SDL_Rect rect{coord.x, coord.y, 
                CELL_SIZE, CELL_SIZE};
            SDL_RenderCopy(renderer, 
                    get_matrix({x, y}), nullptr, &rect);
        }
}

static SDL_Texture *player_to_cell(Cell current_player) {
    if (current_player == Cell::AI)
        return X;
    else
        return O;
}

static void move(Position pos) {
    set_matrix(pos, player_to_cell(game_state.current_player()));
    game_state.move(pos);
}

static void next_game() {
    reset_matrix();
    start_player = inverse_of(start_player);
    game_state = State{start_player};

    if (start_player == Cell::AI) {
        auto action = AI_next_move(game_state);
        move(action);
    }

    slide_vector = {0, 0};
    may_drag = false;
    dragging = false;
}

static void run_ai_move_background() {
    thread = std::thread([] {
            ai_thinking = true;
            auto action = AI_next_move(game_state);
            ai_thinking = false;

            std::lock_guard<std::mutex> gaurd{ai_move_mutex};
            ai_moved = true;
            recent_ai_move = action;
    });
    thread.detach();
}

static void handle_human_move(Position pos) {
    if (!(game_state.current_player() == Cell::HUMAN))
        return;
    auto actions = game_state.legal_actions();
    auto it = std::find(actions.begin(), actions.end(), pos);
    if (it == actions.end())
        return;

    move(pos);

    if (game_state.is_terminal()) {
        next_game();
        return;
    }

    run_ai_move_background();
}

static bool handle_game_mouse_event(SDL_Event event) {
    int x = event.motion.x;
    int y = event.motion.y;

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button != SDL_BUTTON_LEFT)
            return false;

        may_drag = true;
        begin_drag = {x, y};
        old_slide_vector = slide_vector;
        return true;
    }
    else if (event.type == SDL_MOUSEMOTION) {
        if (may_drag) {
            auto dx = x - begin_drag.x;
            auto dy = y - begin_drag.y;
            if (std::abs(dx) >= 5 || std::abs(dy) >= 5)
                dragging = true;
        }

        if (dragging) {
            auto dx = x - begin_drag.x;
            auto dy = y - begin_drag.y;
            slide_vector.x = old_slide_vector.x + dx;
            slide_vector.y = old_slide_vector.y + dy;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (!dragging) {
            auto pos = coord_to_pos(Coord{x, y}, slide_vector);
            handle_human_move(pos);
        }

        may_drag = false;
        dragging = false;
    }
    return false;
}

void init_texts() {
    TTF_Init();
    Sans = TTF_OpenFont("./assets/Sans.ttf", 24);
    if (!Sans) {
        std::cout << TTF_GetError() << std::endl;
    }

    auto surface = TTF_RenderText_Solid(Sans, "AI thinking...", {255, 255, 255, 255});

    ai_thinking_msg = SDL_CreateTextureFromSurface(renderer, surface);
    assert(ai_thinking_msg != nullptr);

    ai_thinking_msg_rect.x = 10;
    ai_thinking_msg_rect.y = HEIGHT - surface->h - 10;
    ai_thinking_msg_rect.w = surface->w;
    ai_thinking_msg_rect.h = surface->h;

    SDL_FreeSurface(surface);
}

void destroy_texts() {
    SDL_DestroyTexture(ai_thinking_msg);
}

void display_texts() {
    if (ai_thinking)
        SDL_RenderCopy(renderer, ai_thinking_msg, nullptr, &ai_thinking_msg_rect);
}

} // namespace gomoku
} // namespace game
} // namespace ai

int main() {
    using namespace ai::game::gomoku;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    auto window = SDL_CreateWindow("Gomoku", 
            100, 100, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED);

    init_texts();

    N = IMG_LoadTexture(renderer, "assets/N.png");
    O = IMG_LoadTexture(renderer, "assets/O.png");
    X = IMG_LoadTexture(renderer, "assets/X.png");

    next_game();

    while (true) {
        {
            std::unique_lock<std::mutex> lock{ai_move_mutex};
            if (ai_moved) {
                ai_moved = false;
                auto action = recent_ai_move;
                lock.unlock();

                move(action);
                if (game_state.is_terminal())
                    next_game();

                lock.lock();
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                goto QuitGame;
            handle_game_mouse_event(event);
        }
        SDL_RenderClear(renderer);
        display_matrix();
        display_texts();
        SDL_RenderPresent(renderer);
    }

QuitGame:
    destroy_texts();

    SDL_DestroyTexture(N);
    SDL_DestroyTexture(O);
    SDL_DestroyTexture(X);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
