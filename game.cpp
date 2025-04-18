#include "game.h"
#include <SDL_image.h>

Game::Game() : gameFrameTexture(nullptr), renderer(nullptr) {}

Game::~Game() {
    if (gameFrameTexture) {
        SDL_DestroyTexture(gameFrameTexture);
    }
}

void Game::init(SDL_Renderer* renderer) {
    this->renderer = renderer;
    
    // Load game frame texture
    SDL_Surface* surface = IMG_Load("assets/image/screens/game_frame.png");
    if (surface) {
        gameFrameTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void Game::setCurrentMap(const std::string& mapType) {
    if (mapType == "basketball") {
        currentMap.loadMap("assets/maps/basketball.txt", "assets/image/tiles/basketballcourt/", renderer);
    } else if (mapType == "tomb") {
        currentMap.loadMap("assets/maps/tomb.txt", "assets/image/tiles/egyptiantomb/", renderer);
    }
}

void Game::render(SDL_Renderer* renderer) {
    // Render map first
    currentMap.render(365, 3, 799, 705);
    
    // Then render game frame on top
    SDL_RenderCopy(renderer, gameFrameTexture, nullptr, nullptr);
}

void Game::clean() {
    if (gameFrameTexture) {
        SDL_DestroyTexture(gameFrameTexture);
        gameFrameTexture = nullptr;
    }
    // Clean up map resources if needed
    currentMap.clean();
}
