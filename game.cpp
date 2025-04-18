#include "game.h"
#include <SDL_image.h>
#include <iostream>

Game::Game() 
    : gameFrameTexture(nullptr),
      renderer(nullptr), 
      pauseButton("Pause", 188, 607, 140, 40),
      audioManager(nullptr),
      isPaused(false) {}

Game::~Game() {
    clean();
}

void Game::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->renderer = renderer;
    this->audioManager = audioManager;
    
    // Load textures
    SDL_Surface* surface = IMG_Load("assets/image/screens/game_frame.png");
    if (surface) {
        gameFrameTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    pauseButton.setTextures(renderer, 
        "assets/image/buttons/pause_normal.png",
        "assets/image/buttons/pause_hover.png");
    pauseButton.setAudio(audioManager);
    
    pause.init(renderer, audioManager);
}

void Game::setCurrentMap(const std::string& mapType) {
    if (mapType == "basketball") {
        currentMap.loadMap("assets/maps/basketball.txt", "assets/image/tiles/basketballcourt/", renderer);
    } else if (mapType == "tomb") {
        currentMap.loadMap("assets/maps/tomb.txt", "assets/image/tiles/egyptiantomb/", renderer);
    }
}

void Game::handleEvents(SDL_Event& event, ScreenState& currentState) {
    if (!isPaused) {
        if (pauseButton.handleEvent(event)) {
            if (event.type == SDL_MOUSEMOTION) {
                audioManager->playSound("assets/audio/hover.mp3");
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                audioManager->playSound("assets/audio/click.mp3");
                isPaused = true;
            }
        }
    } else {
        pause.handleEvents(event, currentState, isPaused);
    }
}

void Game::render(SDL_Renderer* renderer) {
    // Render map
    currentMap.render(365, 3, 799, 705);
    
    // Render game frame
    SDL_RenderCopy(renderer, gameFrameTexture, nullptr, nullptr);
    
    // Render pause button or pause screen
    if (!isPaused) {
        pauseButton.render(renderer);
    } else {
        pause.render(renderer);
    }
}

void Game::clean() {
    if (gameFrameTexture) {
        SDL_DestroyTexture(gameFrameTexture);
        gameFrameTexture = nullptr;
    }
    currentMap.clean();
    pauseButton.clean();
    pause.clean();
}
