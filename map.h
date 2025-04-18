#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <SDL.h>

class Map {
public:
    static const int MAP_WIDTH = 17;
    static const int MAP_HEIGHT = 15;
    static const int TILE_SIZE = 47;

    Map();
    ~Map();
    void loadMap(const std::string& mapFile, const std::string& tilePath, SDL_Renderer* renderer);
    void render(int x, int y, int width, int height);
    void clean();

private:
    void loadMapFromTxt(const std::string& filename);
    void renderMapFromTxt();
    std::vector<std::vector<int>> mapData;
    SDL_Texture* tiles[6];  // Array of 6 tile textures
    SDL_Renderer* renderer;
};

#endif
