#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WIDTH 600
#define HEIGHT 600

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    auto window = SDL_CreateWindow("Gomoku", 
            100, 100, WIDTH, HEIGHT, 0);
    auto renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED);
    auto image = IMG_LoadTexture(renderer, "assets/X.png");

    int w, h;
    SDL_QueryTexture(image, nullptr, nullptr, &w, &h);
    SDL_Rect rect = { 0, 0, 30, 30 };

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                goto QuitGame;
            else if (event.type == SDL_MOUSEBUTTONUP) {
                std::cout << "Up" << std::endl;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                std::cout << "Down" << std::endl;
            }
            else if (event.type == SDL_MOUSEMOTION) {
                std::cout << "Motion" << std::endl;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, image, nullptr, &rect);

        SDL_RenderPresent(renderer);
    }

QuitGame:
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
