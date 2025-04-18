#include "include/SDL.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "GameManager.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "menu.h"
#include "how_to_play.h"
#include "intro.h"
#include "game_settings.h"
#include "game.h" // Thêm include game.h

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Bomb It", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize AudioManager
    AudioManager audioManager;
    if (!audioManager.init()) {
        std::cerr << "Failed to initialize AudioManager" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize screens
    Menu menu;
    HowToPlay howToPlay;
    Intro intro;
    Game game;
    GameSettings gameSettings(&game);

    menu.init(renderer, &audioManager);
    howToPlay.init(renderer, &audioManager);
    intro.init(renderer, &audioManager);
    gameSettings.init(renderer, &audioManager);
    game.init(renderer);

    ScreenState currentState = ScreenState::MENU;
    bool running = true;
    SDL_Event event;

    // Main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Handle events based on current screen
            if (currentState == ScreenState::MENU) {
                menu.handleEvents(event, currentState);
                if (currentState == ScreenState::INTRO) {
                    intro.reset(); // Reset intro state when transitioning from MENU to INTRO
                }
            } else if (currentState == ScreenState::HOW_TO_PLAY) {
                howToPlay.handleEvents(event, currentState);
            } else if (currentState == ScreenState::INTRO) {
                intro.handleEvents(event, currentState);
            } else if (currentState == ScreenState::GAME_SETTINGS) {
                gameSettings.handleEvents(event, currentState);
                if (currentState == ScreenState::MENU) {
                    gameSettings.reset(); // Reset game settings when transitioning back to MENU
                }
            } else if (currentState == ScreenState::GAME) {
                // Xử lý sự kiện cho màn hình game
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = ScreenState::GAME_SETTINGS;
                }
            }
        }

        // Render based on current screen
        SDL_RenderClear(renderer);
        if (currentState == ScreenState::MENU) {
            menu.render(renderer);
        } else if (currentState == ScreenState::HOW_TO_PLAY) {
            howToPlay.render(renderer);
        } else if (currentState == ScreenState::INTRO) {
            intro.update(currentState); // Pass currentState to handle transition
            intro.render(renderer);
        } else if (currentState == ScreenState::GAME_SETTINGS) {
            gameSettings.render(renderer);
        } else if (currentState == ScreenState::GAME) {
            game.render(renderer);
        }
        SDL_RenderPresent(renderer);
    }

    // Clean up
    menu.clean();
    howToPlay.clean();
    intro.clean();
    gameSettings.clean();
    game.clean(); // Thêm dọn dẹp game
    audioManager.clean(); // Dọn dẹp AudioManager
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

