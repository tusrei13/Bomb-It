#include "game.h"
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <algorithm>

Game::Game() 
    : gameFrameTexture(nullptr),
      renderer(nullptr), 
      pauseButton("Pause", 188, 607, 140, 40),
      audioManager(nullptr),
      player1(false),
      player2(false),
      player1CanPlaceBomb(true),
      player2CanPlaceBomb(true),
      isPaused(false) {}

Game::~Game() {
    clean();
}

void Game::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->renderer = renderer;
    this->audioManager = audioManager;
    
    // Load frame UI
    SDL_Surface* surface = IMG_Load("assets/image/screens/game_frame.png");
    if (surface) {
        gameFrameTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    // Setup pause button
    pauseButton.setTextures(renderer, 
        "assets/image/buttons/pause_normal.png",
        "assets/image/buttons/pause_hover.png");
    pauseButton.setAudio(audioManager);
    
    // Initialize pause screen
    pause.init(renderer, audioManager);

    // Initialize player1
    player1.init(renderer, "assets/image/players/player1/player1_spritesheet.png", 412, 50);
    
    // Initialize player2
    player2.init(renderer, "assets/image/players/player2/player2_spritesheet.png", 1070, 614);

    // Load bomb spritesheet
    loadBombTexture();
}

void Game::loadBombTexture() {
    SDL_Surface* surface = IMG_Load("assets/image/bomb/bomb.png");
    if (!surface) {
        std::cerr << "Failed to load bomb.png: " << IMG_GetError() << std::endl;
        return;
    }
    
    // Set color key to handle transparency - black color will be transparent
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    
    bombTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!bombTexture) {
        std::cerr << "Failed to create bomb texture: " << SDL_GetError() << std::endl;
    }
    
    // Set blend mode to handle transparency
    SDL_SetTextureBlendMode(bombTexture, SDL_BLENDMODE_BLEND);
    
    SDL_FreeSurface(surface);
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

        // Handle players movement and bombs
        const Uint8* keyState = SDL_GetKeyboardState(nullptr);
        
        // Handle player movements with bomb collision
        player1.handleInput(keyState, currentMap.getData(), bombsPlayer1, false);
        player2.handleInput(keyState, currentMap.getData(), bombsPlayer2, true);
        
        // Xử lý đặt bomb cho player1
        if (keyState[SDL_SCANCODE_SPACE] && player1CanPlaceBomb) {
            int tileX = (player1.getDestRect().x - 365 + Player::getFrameWidth()/2) / 47;
            int tileY = (player1.getDestRect().y - 3 + Player::getFrameHeight()/2) / 47;
            
            if (currentMap.getData()[tileY][tileX] == 0) {  // Chỉ cho đặt bomb trên ô floor
                bombsPlayer1.push_back(std::make_shared<Bomb>(tileX, tileY, bombTexture));
                player1.setLastPlacedBombPosition(tileX, tileY);
                player1CanPlaceBomb = false;
            }
        }

        // Handle player2 movement and bomb placement
        static bool player2CanPlaceBomb = true;
        if (keyState[SDL_SCANCODE_RETURN] && player2CanPlaceBomb) {
            int tileX = (player2.getDestRect().x - 365) / 47;
            int tileY = (player2.getDestRect().y - 3) / 47;
            
            if (currentMap.getData()[tileY][tileX] == 0) {  // Chỉ cho đặt bomb trên ô floor
                bombsPlayer2.push_back(std::make_shared<Bomb>(tileX, tileY, bombTexture));
                player2.setLastPlacedBombPosition(tileX, tileY);
                player2CanPlaceBomb = false;
            }
        }
    } else {
        pause.handleEvents(event, currentState, isPaused);
    }
}

void Game::update() {
    if (!isPaused) {
        player1.updateFrame();
        player2.updateFrame();

        // Update bombs của player1
        auto it1 = bombsPlayer1.begin();
        while (it1 != bombsPlayer1.end()) {
            (*it1)->update();
            if ((*it1)->isExpired()) {
                int tileX = ((*it1)->getDestRect().x - 365) / 47;
                int tileY = ((*it1)->getDestRect().y - 3) / 47;
                
                // Tạo explosion trước khi xóa bomb
                auto explosion = std::make_unique<Explosion>(tileX, tileY, 2, currentMap);
                explosion->loadTextures(renderer);
                explosions.push_back(std::move(explosion));

                // Chỉ xóa bomb không thay đổi map tile
                player1.clearLastPlacedBombPosition();
                it1 = bombsPlayer1.erase(it1);
                player1CanPlaceBomb = true;
                audioManager->playSound("assets/audio/bomb_explode.mp3");
            } else {
                ++it1;
            }
        }

        // Update bombs của player2 - tương tự như player1
        auto it2 = bombsPlayer2.begin();
        while (it2 != bombsPlayer2.end()) {
            (*it2)->update();
            if ((*it2)->isExpired()) {
                int tileX = ((*it2)->getDestRect().x - 365) / 47;
                int tileY = ((*it2)->getDestRect().y - 3) / 47;
                
                // Tạo explosion trước khi xóa bomb
                auto explosion = std::make_unique<Explosion>(tileX, tileY, 2, currentMap);
                explosion->loadTextures(renderer);
                explosions.push_back(std::move(explosion));

                // Sau đó mới xóa bomb và cập nhật map
                player2.clearLastPlacedBombPosition();
                it2 = bombsPlayer2.erase(it2);
                player2CanPlaceBomb = true;
                audioManager->playSound("assets/audio/bomb_explode.mp3");
            } else {
                ++it2;
            }
        }

        // Update explosions
        auto expIt = explosions.begin();
        while (expIt != explosions.end()) {
            (*expIt)->update();
            if ((*expIt)->isFinished()) {
                // Convert brick tiles to floor tiles
                for (const auto& part : (*expIt)->getParts()) {
                    (*expIt)->convertBrickToFloor(currentMap, part.gridX, part.gridY);
                }
                expIt = explosions.erase(expIt);
            } else {
                ++expIt;
            }
        }
    }
}

void Game::render(SDL_Renderer* renderer) {
    // Render map
    currentMap.render(365, 3, 799, 705);
    
    // Render bombs với debug
    for (const auto& bomb : bombsPlayer1) {
        bomb->render(renderer);
    }
    for (const auto& bomb : bombsPlayer2) {
        bomb->render(renderer);
    }
    
    // Render explosions
    for (const auto& explosion : explosions) {
        explosion->render(renderer);
    }
    
    // Render player
    player1.render(renderer);
    player2.render(renderer);

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
    if (bombTexture) {
        SDL_DestroyTexture(bombTexture);
        bombTexture = nullptr;
    }
    currentMap.clean();
    pauseButton.clean();
    pause.clean();
    explosions.clear();
}
