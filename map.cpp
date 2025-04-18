#include "map.h"
#include <fstream>
#include <iostream>
#include <SDL_image.h>

Map::Map() : renderer(nullptr) {
    for (int i = 0; i < 6; i++) {
        tiles[i] = nullptr;
    }
}

Map::~Map() {
    clean();
}

void Map::loadMapFromTxt(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << filename << std::endl;
        return;
    }

    mapData.resize(MAP_HEIGHT, std::vector<int>(MAP_WIDTH));
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            file >> mapData[y][x];
        }
    }
    file.close();
}

void Map::loadMap(const std::string& mapFile, const std::string& tilePath, SDL_Renderer* renderer) {
    this->renderer = renderer;
    
    // Load map data
    loadMapFromTxt(mapFile);
    
    // Load tile textures
    const char* tileFiles[] = {
        "floor.png", "brick_1.png", "wall_1.png",
        "brick_2.png", "wall_2.png", "brick_3.png"
    };

    for (int i = 0; i < 6; i++) {
        std::string fullPath = tilePath + tileFiles[i];
        SDL_Surface* surface = IMG_Load(fullPath.c_str());
        if (!surface) {
            std::cerr << "Failed to load tile: " << fullPath << std::endl;
            continue;
        }
        tiles[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void Map::renderMapFromTxt() {
    SDL_Rect destRect = { 365, 3, TILE_SIZE, TILE_SIZE };
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            destRect.x = 365 + x * TILE_SIZE;
            destRect.y = 3 + y * TILE_SIZE;

            int tileId = mapData[y][x];
            if (tileId >= 0 && tileId <= 5 && tiles[tileId]) {
                SDL_RenderCopy(renderer, tiles[tileId], nullptr, &destRect);
            }
        }
    }
}

void Map::render(int x, int y, int width, int height) {
    renderMapFromTxt();
}

void Map::clean() {
    for (int i = 0; i < 6; i++) {
        if (tiles[i]) {
            SDL_DestroyTexture(tiles[i]);
            tiles[i] = nullptr;
        }
    }
    mapData.clear();
    renderer = nullptr;
}
