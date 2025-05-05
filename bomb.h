#ifndef BOMB_H
#define BOMB_H

#include <SDL.h>
#include <string>

class Bomb {
public:
    Bomb(int gridX, int gridY, SDL_Texture* bombTexture);
    ~Bomb();

    void update();
    void render(SDL_Renderer* renderer);
    bool isExpired() const;
    const SDL_Rect& getDestRect() const { return destRect; }
    // Xóa hàm loadTexture static

private:
    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    int currentFrame;
    Uint32 lastFrameTime;
    bool m_active;
    int animationTimer;
    static const int BOMB_LIFETIME = 5000; // 5 seconds in milliseconds
    Uint32 createTime;

    static const int FRAME_WIDTH = 47;     // Chiều rộng mỗi frame 
    static const int FRAME_HEIGHT = 47;    // Chiều cao mỗi frame
    static const int TOTAL_FRAMES = 1;     // Chỉ có 1 frame cho bomb.png
    static const int FRAME_DELAY = 100;    // 100ms/frame
    static const int SPRITE_SHEET_WIDTH = 2444;  // Tổng chiều rộng của sprite sheet
    void updateAnimation();
};

#endif
