#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "map.h"

class Game {
public:
    Game();
    ~Game();
    void init(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer);
    void setCurrentMap(const std::string& mapType); // basketball or tomb
    void clean(); // Thêm phương thức clean

private:
    Map currentMap;
    SDL_Texture* gameFrameTexture;
    SDL_Renderer* renderer;
};

#endif
