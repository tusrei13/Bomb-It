#include "tiles.h"
#include <SDL_image.h>
#include <iostream>

Tile::Tile(const std::string& texturePath, SDL_Renderer* renderer) : texture(nullptr), renderer(renderer) {
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        std::cerr << "Failed to load tile texture: " << texturePath << std::endl;
    }
}

Tile::~Tile() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Tile::render(int x, int y, int width, int height) {
    if (texture && renderer) {
        SDL_Rect destRect = {x, y, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }
}
