#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SDL.h>
#include <vector>
#include "map.h"

struct ExplosionPart {
    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    int currentFrame;
    int gridX;
    int gridY;
};

class Explosion {
public:
    Explosion(int x, int y, int range, const Map& map);
    ~Explosion();

    void loadTextures(SDL_Renderer* renderer);
    void update();
    void render(SDL_Renderer* renderer);
    bool isFinished() const;
    bool isValidExplosion(int x, int y, const Map& map) const;
    void convertBrickToFloor(Map& map, int x, int y);
    const std::vector<ExplosionPart>& getParts() const { return parts; }

private:
    int gridX, gridY;
    int range;
    bool finished;
    Uint32 startTime;
    std::vector<ExplosionPart> parts;
    const Map& gameMap;
    int currentFrame; // Thêm biến này để đồng bộ frame
    
    static const int FRAME_WIDTH = 47;
    static const int FRAME_HEIGHT = 47;
    static const int TOTAL_FRAMES = 3;
    static const int FRAME_DELAY = 100;
    static const int EXPLOSION_DURATION = 300;
};

#endif
