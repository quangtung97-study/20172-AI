#include <ai/game/gomoku/SDLWrapper.hpp>
#include <algorithm>
#include <SDL2/SDL_ttf.h>
#include <cassert>

namespace ai {
namespace game {
namespace gomoku {

void SDLWrapper::move(Action action) {
    (*matrix_renderer_)(action) = player_to_cell(state().current_player());
    if (state().current_player() == Cell::HUMAN) {
        if (game_->ai_prev_move_.has_value())
            (*matrix_renderer_)(game_->ai_prev_move_.value()) = matrix_renderer_->X;
    }
    else {
        game_->ai_prev_move_ = action;
    }
    state().move(action);
}

void SDLWrapper::next_game() {
    matrix_renderer_->reset();
    start_player_ = inverse_of(start_player_);
    game_ = std::make_unique<Game>(start_player_);
    ai_mover_ = std::make_unique<AIMover>(game_->state_);

    if (start_player_ == Cell::AI) {
        auto action = AI_next_move(state());
        move(action);
    }
    else {
        (*matrix_renderer_)({0, 0}) = matrix_renderer_->N_gray;
    }

    may_drag_ = false;
    dragging_  = false;
}

SDL_Texture *SDLWrapper::player_to_cell(Cell player) const {
    if (player == Cell::AI)
        return matrix_renderer_->X_gray;
    else
        return matrix_renderer_->O;
}

void SDLWrapper::handle_human_move(Position pos) {
    if (game_->going_to_next_game_)
        return;

    if (!(state().current_player() == Cell::HUMAN))
        return;
    auto actions = state().legal_actions();
    auto it = std::find(actions.begin(), actions.end(), pos);
    if (it == actions.end())
        return;

    move(pos);

    if (state().is_terminal()) {
        game_->you_won_displayed_ = true;
        game_->going_to_next_game_ = true;
        return;
    }

    ai_mover_->next_move_in_background();
}

bool SDLWrapper::handle_game_mouse_event(SDL_Event event) {
    int x = event.motion.x;
    int y = event.motion.y;

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button != SDL_BUTTON_LEFT)
            return false;

        may_drag_ = true;
        begin_drag_ = {x, y};

        matrix_renderer_->begin_slide();
        return true;
    }
    else if (event.type == SDL_MOUSEMOTION) {
        if (may_drag_) {
            auto dx = x - begin_drag_.x;
            auto dy = y - begin_drag_.y;
            if (std::abs(dx) >= 5 || std::abs(dy) >= 5)
                dragging_ = true;
        }

        if (dragging_) {
            auto dx = x - begin_drag_.x;
            auto dy = y - begin_drag_.y;
            matrix_renderer_->slide({dx, dy});
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (!dragging_) {
            auto pos = matrix_renderer_->coord_to_pos(Coord{x, y});
            handle_human_move(pos);
        }

        if (game_->going_to_next_game_) {
            next_game();
        }

        may_drag_ = false;
        dragging_  = false;
    }
    return false;
}

void SDLWrapper::init_texts() {
    TTF_Init();
    Sans_ = TTF_OpenFont("assets/Sans.ttf", 20);
    if (!Sans_)
        throw std::runtime_error(TTF_GetError());
}

void SDLWrapper::display_texts() {
    guide_->render();

    if (ai_mover_->thinking())
        ai_thinking_->render();
    if (game_->ai_won_displayed_)
        ai_won_->render();
    if (game_->you_won_displayed_)
        you_won_->render();
}

SDLWrapper::SDLWrapper() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("Can't init SDL");

    const auto screen_width = 600;
    const auto screen_height = 600;

    window_ = SDL_CreateWindow(
            "Gomoku", 100, 100, screen_width, screen_height, 0);

    renderer_ = SDL_CreateRenderer(
            window_, -1, SDL_RENDERER_ACCELERATED);

    init_texts();

    matrix_renderer_ = std::make_unique<MatrixRenderer>(renderer_, screen_width, screen_height);

    ai_thinking_ = std::make_unique<TextRenderer>(
            Sans_, renderer_, "AI thinking...", SDL_Color{255, 255, 255, 255}, 10, 560);

    ai_won_ = std::make_unique<TextRenderer>(
            Sans_, renderer_, "AI won!", SDL_Color{255, 255, 255, 255}, 10, 15);

    you_won_ = std::make_unique<TextRenderer>(
            Sans_, renderer_, "you won!", SDL_Color{255, 255, 255, 255}, 10, 15);

    guide_ = std::make_unique<TextRenderer>(
            Sans_, renderer_, "F5 to reset", SDL_Color{255, 255, 255, 255}, 
            screen_width - 100, 15);

    next_game();
}

void SDLWrapper::run() {
    while (true) {
        if (ai_mover_->moved()) {
            auto action = ai_mover_->recent_move();
            move(action);

            if (state().is_terminal()) {
                game_->ai_won_displayed_ = true;
                game_->going_to_next_game_ = true;
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                return;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F5)
                    next_game();
            }
            handle_game_mouse_event(event);
        }

        SDL_RenderClear(renderer_);
        matrix_renderer_->render();
        display_texts();
        SDL_RenderPresent(renderer_);
    }
}

SDLWrapper::~SDLWrapper() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
}

// Text Renderer
TextRenderer::TextRenderer(TTF_Font *font, 
        SDL_Renderer *renderer,
        const std::string& string, 
        SDL_Color color,
        int x, int y)
{
    renderer_ = renderer;
    auto surface = TTF_RenderText_Solid(font, string.c_str(), color);

    texture_ = SDL_CreateTextureFromSurface(renderer_, surface);
    assert(texture_ != nullptr);

    rect_.x = x;
    rect_.y = y;
    rect_.w = surface->w;
    rect_.h = surface->h;

    SDL_FreeSurface(surface);
}

void TextRenderer::render() {
    SDL_RenderCopy(renderer_, texture_, nullptr, &rect_);
}

TextRenderer::~TextRenderer() {
    SDL_DestroyTexture(texture_);
}

} // namespace gomoku
} // namespace game
} // namespace ai
