#include "bomb.h"
#include <SDL_image.h>
#include <iostream>

// Xóa BOMB_SPRITE_PATH vì đã được xử lý trong Game::loadBombTexture()

Bomb::Bomb(int gridX, int gridY, SDL_Texture* bombTexture) 
    : texture(bombTexture), 
      currentFrame(0), 
      lastFrameTime(0),
      createTime(SDL_GetTicks()),
      m_active(true) {
    
    if (!texture) {
        std::cerr << "Warning: Bomb created with null texture!" << std::endl;
    }
    
    // Bắt đầu với frame đầu tiên từ sprite sheet
    srcRect = {
        0,              // X bắt đầu từ 0
        0,              // Y luôn là 0 vì sprite sheet chỉ có 1 hàng
        FRAME_WIDTH,    // Chiều rộng frame
        FRAME_HEIGHT    // Chiều cao frame
    };
    
    // Set vị trí hiển thị bomb trên map
    destRect = {
        365 + (gridX * FRAME_WIDTH),  
        3 + (gridY * FRAME_HEIGHT),  
        FRAME_WIDTH,                  
        FRAME_HEIGHT
    };
}

Bomb::~Bomb() {
    // Không cần giải phóng texture vì nó được quản lý bởi Game
}

void Bomb::update() {
    if (!m_active) return;

    Uint32 currentTime = SDL_GetTicks();
    
    // Kiểm tra thời gian tồn tại
    if (currentTime - createTime >= BOMB_LIFETIME) {
        m_active = false;  // Đánh dấu bomb đã hết hạn
        return;
    }

    // Cập nhật animation
    if (currentTime - lastFrameTime >= FRAME_DELAY) {
        currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
        srcRect.x = currentFrame * FRAME_WIDTH;
        lastFrameTime = currentTime;
    }
}

void Bomb::render(SDL_Renderer* renderer) {
    if (!m_active || !texture) return;
    
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

bool Bomb::isExpired() const {
    return !m_active;
}

// Xóa hàm loadTexture vì không cần thiết nữa