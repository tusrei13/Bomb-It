#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <vector>
#include <string>
#include <memory>
#include "bomb.h"  // Add this for Bomb class

class Player {
public:
    enum Direction {
        DOWN = 0,
        LEFT = 1,
        RIGHT = 2,
        UP = 3
    };

    explicit Player(bool isSecondPlayer = false);
    ~Player();
    void init(SDL_Renderer* renderer, const std::string& spritePath, int startX, int startY);
    void handleInput(const Uint8* keyState, const std::vector<std::vector<int>>& mapData, const std::vector<std::shared_ptr<Bomb>>& bombs, bool useArrowKeys);
    void updateAnimation();
    void updateFrame();
    void render(SDL_Renderer* renderer);
    
    int getX() const { return destRect.x; }
    int getY() const { return destRect.y; }
    int getWidth() const { return destRect.w; }
    int getHeight() const { return destRect.h; }
    static int getFrameWidth() { return FRAME_WIDTH; }
    static int getFrameHeight() { return FRAME_HEIGHT; }
    const SDL_Rect& getDestRect() const { return destRect; }
    
    void setLastPlacedBombPosition(int x, int y);
    void clearLastPlacedBombPosition();
    bool canPassBomb(int x, int y) const;
    bool isInMap(int x, int y, const std::vector<std::vector<int>>& mapData);
    bool checkCollision(int newX, int newY, const std::vector<std::vector<int>>& mapData, const std::vector<std::shared_ptr<Bomb>>& bombs);

private:
    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    int currentFrame;
    int frameDelay;
    int frameCounter;
    Direction currentDir;
    bool isMoving;
    int movement_count;
    static const int FRAME_WIDTH = 47;     // Chỉ khai báo một lần
    static const int FRAME_HEIGHT = 47;    // Chỉ khai báo một lần 
    static const int FRAMES_DOWN = 20;
    static const int FRAMES_LEFT = 16;
    static const int FRAMES_RIGHT = 17;
    static const int FRAMES_UP = 23;
    static const int FRAME_DELAY = 12;
    static const int FRAMES_DOWN_P2 = 21;
    static const int FRAMES_LEFT_P2 = 20;
    static const int FRAMES_RIGHT_P2 = 20;
    static const int FRAMES_UP_P2 = 21;
    static constexpr float SPEED = 2.0f;  // Đảm bảo tốc độ là 2 pixel
    static const int MOVEMENT_TO_FRAME = 4;
    bool isPlayer2;
    std::pair<int, int> lastPlacedBombPosition;
    bool isOnOwnBomb;
    bool isBlockedByBomb(int x, int y, const std::vector<std::shared_ptr<Bomb>>& bombs) const;
};

#endif
