#include "explosion.h"
#include <SDL_image.h>
#include <iostream>

Explosion::Explosion(int x, int y, int range, const Map& map)
    : gridX(x), gridY(y), range(range), finished(false), gameMap(map) {
    startTime = SDL_GetTicks(); 
    currentFrame = 0; // Thêm biến này để đồng bộ frame cho tất cả phần explosion
}

Explosion::~Explosion() {
    for (auto& part : parts) {
        SDL_DestroyTexture(part.texture);
    }
    parts.clear();
}

bool Explosion::isValidExplosion(int x, int y, const Map& map) const {
    if (x < 0 || y < 0 || x >= map.getData()[0].size() || y >= map.getData().size()) 
        return false;
        
    int tileValue = map.getData()[y][x];
    return tileValue == 0 || tileValue == 1 || tileValue == 3 || tileValue == 5; // Floor hoặc brick
}

void Explosion::loadTextures(SDL_Renderer* renderer) {
    auto loadExplosionTexture = [renderer](const char* path) {
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) {
            std::cerr << "Failed to load explosion texture: " << path << std::endl;
            return (SDL_Texture*)nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    };

    // Center explosion
    if (isValidExplosion(gridX, gridY, gameMap)) {
        ExplosionPart center;
        center.texture = loadExplosionTexture("assets/image/explosions/explosion_middle.png");
        center.srcRect = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
        center.destRect = {365 + gridX * FRAME_WIDTH, 3 + gridY * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
        center.currentFrame = 0;
        center.gridX = gridX;
        center.gridY = gridY;
        parts.push_back(center);
    }

    // Horizontal explosions
    bool canGoLeft = true;
    bool canGoRight = true;

    for (int i = 1; i <= range && (canGoLeft || canGoRight); i++) {
        // Left horizontal
        if (canGoLeft && isValidExplosion(gridX - i, gridY, gameMap)) {
            ExplosionPart left;
            if (i == range) {
                left.texture = loadExplosionTexture("assets/image/explosions/explosion_left_last.png");
            } else {
                left.texture = loadExplosionTexture("assets/image/explosions/explosion_horizontal.png");
            }
            left.srcRect = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
            left.destRect = {365 + (gridX - i) * FRAME_WIDTH, 3 + gridY * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
            left.currentFrame = 0;
            left.gridX = gridX - i;
            left.gridY = gridY;
            parts.push_back(left);
        } else {
            canGoLeft = false;
        }

        // Right horizontal
        if (canGoRight && isValidExplosion(gridX + i, gridY, gameMap)) {
            ExplosionPart right;
            if (i == range) {
                right.texture = loadExplosionTexture("assets/image/explosions/explosion_right_last.png");
            } else {
                right.texture = loadExplosionTexture("assets/image/explosions/explosion_horizontal.png");
            }
            right.srcRect = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
            right.destRect = {365 + (gridX + i) * FRAME_WIDTH, 3 + gridY * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
            right.currentFrame = 0;
            right.gridX = gridX + i;
            right.gridY = gridY;
            parts.push_back(right);
        } else {
            canGoRight = false;
        }
    }

    // Vertical explosions
    bool canGoUp = true;
    bool canGoDown = true;

    for (int i = 1; i <= range && (canGoUp || canGoDown); i++) {
        // Up vertical
        if (canGoUp && isValidExplosion(gridX, gridY - i, gameMap)) {
            ExplosionPart up;
            if (i == range) {
                up.texture = loadExplosionTexture("assets/image/explosions/explosion_up_last.png");
            } else {
                up.texture = loadExplosionTexture("assets/image/explosions/explosion_vertical.png");
            }
            up.srcRect = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
            up.destRect = {365 + gridX * FRAME_WIDTH, 3 + (gridY - i) * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
            up.currentFrame = 0;
            up.gridX = gridX;
            up.gridY = gridY - i;
            parts.push_back(up);
        } else {
            canGoUp = false;
        }

        // Down vertical
        if (canGoDown && isValidExplosion(gridX, gridY + i, gameMap)) {
            ExplosionPart down;
            if (i == range) {
                down.texture = loadExplosionTexture("assets/image/explosions/explosion_down_last.png");
            } else {
                down.texture = loadExplosionTexture("assets/image/explosions/explosion_vertical.png");
            }
            down.srcRect = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
            down.destRect = {365 + gridX * FRAME_WIDTH, 3 + (gridY + i) * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
            down.currentFrame = 0;
            down.gridX = gridX;
            down.gridY = gridY + i;
            parts.push_back(down);
        } else {
            canGoDown = false;
        }
    }
}

void Explosion::update() {
    Uint32 currentTime = SDL_GetTicks();
    
    if (currentTime - startTime >= EXPLOSION_DURATION) {
        finished = true;
        return;
    }

    // Cập nhật frame chung cho tất cả phần explosion
    currentFrame = ((currentTime - startTime) / FRAME_DELAY) % TOTAL_FRAMES;
    for (auto& part : parts) {
        part.srcRect.x = currentFrame * FRAME_WIDTH;
    }
}

void Explosion::convertBrickToFloor(Map& map, int x, int y) {
    int tileValue = map.getData()[y][x];
    if (tileValue == 1 || tileValue == 3 || tileValue == 5) { // Brick tiles
        map.getMutableData()[y][x] = 0; // Convert to floor
    }
}

void Explosion::render(SDL_Renderer* renderer) {
    for (const auto& part : parts) {
        SDL_RenderCopy(renderer, part.texture, &part.srcRect, &part.destRect);
    }
}

bool Explosion::isFinished() const {
    return finished;
}