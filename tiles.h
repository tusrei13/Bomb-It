#ifndef TILES_H
#define TILES_H

#include <SDL.h>
#include <string>

class Tile {
public:
    Tile(const std::string& texturePath, SDL_Renderer* renderer);
    ~Tile();
    void render(int x, int y, int width, int height);

private:
    SDL_Texture* texture;
    SDL_Renderer* renderer;
};

#endif
