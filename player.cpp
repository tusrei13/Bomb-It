#include "player.h"
#include "map.h"
#include <SDL_image.h>
#include <iostream>

// Định nghĩa số lượng khung hình tối đa cho mỗi hướng di chuyển
#define FRAMES_PER_DIRECTION 4

Player::Player(bool isSecondPlayer) 
    : texture(nullptr), 
      currentFrame(0), 
      frameDelay(FRAME_DELAY),  // Sử dụng FRAME_DELAY mới
      frameCounter(0), 
      currentDir(DOWN),
      isMoving(false),
      movement_count(0) {
      
    lastPlacedBombPosition = std::make_pair(-1, -1);
    isOnOwnBomb = false;
      
      // Set vị trí ban đầu dựa trên loại player
      if (isSecondPlayer) {
          destRect.x = 1070;
          destRect.y = 614;
      } else {
          destRect.x = 412;
          destRect.y = 50;
      }
      destRect.w = FRAME_WIDTH;
      destRect.h = FRAME_HEIGHT;
    // Set initial sprite frame (first frame of DOWN animation)
    srcRect.x = 0;  // Start with first frame
    srcRect.y = 0;  // Start with DOWN animation
    srcRect.w = FRAME_WIDTH;
    srcRect.h = FRAME_HEIGHT;
}

bool Player::canPassBomb(int x, int y) const {
    return isOnOwnBomb && 
           x == lastPlacedBombPosition.first && 
           y == lastPlacedBombPosition.second;
}

Player::~Player() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Player::setLastPlacedBombPosition(int x, int y) {
    lastPlacedBombPosition = {x, y};
    isOnOwnBomb = true;
}

void Player::clearLastPlacedBombPosition() {
    lastPlacedBombPosition = {-1, -1};
    isOnOwnBomb = false;
}

void Player::init(SDL_Renderer* renderer, const std::string& spritePath, int startX, int startY) {
    SDL_Surface* surface = IMG_Load(spritePath.c_str());
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    destRect.x = startX;
    destRect.y = startY;
    destRect.w = FRAME_WIDTH;
    destRect.h = FRAME_HEIGHT;
    
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = FRAME_WIDTH;
    srcRect.h = FRAME_HEIGHT;
    
    this->isPlayer2 = (startX == 1070 && startY == 614);  // Check if this is player2
}

void Player::handleInput(const Uint8* keyState, const std::vector<std::vector<int>>& mapData, const std::vector<std::shared_ptr<Bomb>>& bombs, bool useArrowKeys) {
    int newX = destRect.x;
    int newY = destRect.y;
    isMoving = false;

    // Handle input based on control scheme
    if (useArrowKeys) {
        // Arrow keys for player2
        if (keyState[SDL_SCANCODE_UP]) {
            currentDir = UP;
            newY = destRect.y - SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 141;
        }
        else if (keyState[SDL_SCANCODE_DOWN]) {
            currentDir = DOWN;
            newY = destRect.y + SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 0;
        }
        else if (keyState[SDL_SCANCODE_LEFT]) {
            currentDir = LEFT;
            newX = destRect.x - SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 47;
        }
        else if (keyState[SDL_SCANCODE_RIGHT]) {
            currentDir = RIGHT;
            newX = destRect.x + SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 94;
        }
    } else {
        // WASD for player1
        if (keyState[SDL_SCANCODE_W]) {
            currentDir = UP;
            newY = destRect.y - SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 141;
        }
        else if (keyState[SDL_SCANCODE_S]) {
            currentDir = DOWN;
            newY = destRect.y + SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 0;
        }
        else if (keyState[SDL_SCANCODE_A]) {
            currentDir = LEFT;
            newX = destRect.x - SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 47;
        }
        else if (keyState[SDL_SCANCODE_D]) {
            currentDir = RIGHT;
            newX = destRect.x + SPEED; // Sử dụng SPEED từ class
            isMoving = true;
            srcRect.y = 94;
        }
    }

    if (isMoving) {
        movement_count++;
        if (movement_count >= MOVEMENT_TO_FRAME) {
            movement_count = 0;
            currentFrame++;
            if (currentFrame >= FRAMES_PER_DIRECTION) {
                currentFrame = 0;
            }
        }
        srcRect.x = currentFrame * FRAME_WIDTH;

        // Check collision and update position
        bool canMove = checkCollision(newX, newY, mapData, bombs);
        if (canMove) {
            destRect.x = newX;
            destRect.y = newY;
        } else {
            // Nếu không thể di chuyển, giữ nguyên vị trí nhưng vẫn chạy sprite
            newX = destRect.x;
            newY = destRect.y;
        }
    }
}

void Player::updateAnimation() {
    currentFrame++;
    
    // Reset frame counter based on direction
    switch (currentDir) {
        case DOWN:
            if (currentFrame >= FRAMES_DOWN) currentFrame = 0;
            break;
        case LEFT:
            if (currentFrame >= FRAMES_LEFT) currentFrame = 0;
            break;
        case RIGHT:
            if (currentFrame >= FRAMES_RIGHT) currentFrame = 0;
            break;
        case UP:
            if (currentFrame >= FRAMES_UP) currentFrame = 0;
            break;
    }

    srcRect.x = currentFrame * FRAME_WIDTH;
}

bool Player::isInMap(int x, int y, const std::vector<std::vector<int>>& mapData) {
    return y >= 0 && y < mapData.size() && x >= 0 && x < mapData[0].size();
}

bool Player::isBlockedByBomb(int x, int y, const std::vector<std::shared_ptr<Bomb>>& bombs) const {
    for (const auto& bomb : bombs) {
        int bombX = (bomb->getDestRect().x - 365) / FRAME_WIDTH;
        int bombY = (bomb->getDestRect().y - 3) / FRAME_HEIGHT;
        if (x == bombX && y == bombY) {
            if (!isOnOwnBomb || 
                bombX != lastPlacedBombPosition.first || 
                bombY != lastPlacedBombPosition.second) {
                return true; // Blocked by this bomb
            }
        }
    }
    return false;
}

bool Player::checkCollision(int newX, int newY, const std::vector<std::vector<int>>& mapData, const std::vector<std::shared_ptr<Bomb>>& bombs) {
    int topLeftX = (newX - 365) / FRAME_WIDTH;
    int topLeftY = (newY - 3) / FRAME_HEIGHT;
    int topRightX = (newX - 365 + FRAME_WIDTH - 1) / FRAME_WIDTH;
    int bottomLeftY = (newY - 3 + FRAME_HEIGHT - 1) / FRAME_HEIGHT;

    // Check map collisions
    for (const auto& pos : {
        std::make_pair(topLeftX, topLeftY),
        std::make_pair(topRightX, topLeftY),
        std::make_pair(topLeftX, bottomLeftY),
        std::make_pair(topRightX, bottomLeftY)
    }) {
        if (!isInMap(pos.first, pos.second, mapData)) {
            return false;
        }

        int tileValue = mapData[pos.second][pos.first];
        if (tileValue >= 1 && tileValue <= 5) {
            return false;
        }

        // Check bomb collisions
        if (isBlockedByBomb(pos.first, pos.second, bombs)) {
            return false;
        }
    }

    // Update isOnOwnBomb status
    if (isOnOwnBomb) {
        bool stillOnBomb = false;
        for (const auto& pos : {
            std::make_pair(topLeftX, topLeftY),
            std::make_pair(topRightX, topLeftY),
            std::make_pair(topLeftX, bottomLeftY),
            std::make_pair(topRightX, bottomLeftY)
        }) {
            if (pos.first == lastPlacedBombPosition.first && 
                pos.second == lastPlacedBombPosition.second) {
                stillOnBomb = true;
                break;
            }
        }
        if (!stillOnBomb) {
            isOnOwnBomb = false;
        }
    }

    return true;
}

void Player::updateFrame() {
    if (!isMoving) {
        return;
    }

    frameCounter++;
    if (frameCounter >= FRAME_DELAY) {  // Sử dụng FRAME_DELAY lớn hơn
        frameCounter = 0;
        currentFrame++;

        switch (currentDir) {
            case DOWN:
                if (currentFrame >= FRAMES_DOWN) currentFrame = 0;
                break;
            case LEFT:
                if (currentFrame >= FRAMES_LEFT) currentFrame = 0;
                break;
            case RIGHT:
                if (currentFrame >= FRAMES_RIGHT) currentFrame = 0;
                break;
            case UP:
                if (currentFrame >= FRAMES_UP) currentFrame = 0;
                break;
        }

        srcRect.x = currentFrame * FRAME_WIDTH;
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}
