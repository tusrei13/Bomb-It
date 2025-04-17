#include "buttons.h"
#include "include/SDL_image.h"
#include <iostream>

Button::Button(const std::string& text, int x, int y, int width, int height, bool hasClickState)
    : text(text), x(x), y(y), width(width), height(height),
      normalTexture(nullptr), hoverTexture(nullptr), clickTexture(nullptr),
      hovered(false), clicked(false), hasClickState(hasClickState),
      audioManager(nullptr), hoverPlayed(false) {
    position = {x, y, width, height}; // Khởi tạo vị trí và kích thước của nút
}

Button::~Button() {}

void Button::setTextures(SDL_Renderer* renderer, const std::string& normalPath, const std::string& hoverPath, const std::string& clickPath) {
    SDL_Surface* surface = IMG_Load(normalPath.c_str());
    if (!surface) {
        std::cerr << "Failed to load " << normalPath << ": " << IMG_GetError() << std::endl;
        return;
    }
    normalTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load(hoverPath.c_str());
    if (!surface) {
        std::cerr << "Failed to load " << hoverPath << ": " << IMG_GetError() << std::endl;
        return;
    }
    hoverTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (hasClickState && !clickPath.empty()) {
        surface = IMG_Load(clickPath.c_str());
        if (!surface) {
            std::cerr << "Failed to load " << clickPath << ": " << IMG_GetError() << std::endl;
            return;
        }
        clickTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void Button::setAudio(AudioManager* audioManager) {
    this->audioManager = audioManager;
    hoverPlayed = false;
}

void Button::render(SDL_Renderer* renderer) {
    SDL_Rect rect = {x, y, width, height};
    if (clicked && hasClickState) {
        SDL_RenderCopy(renderer, clickTexture, nullptr, &rect);
    } else if (hovered) {
        SDL_RenderCopy(renderer, hoverTexture, nullptr, &rect);
    } else {
        SDL_RenderCopy(renderer, normalTexture, nullptr, &rect);
    }
}

void Button::resetClick() {
    clicked = false;
}

bool Button::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        bool inside = mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height;

        if (inside) {
            hovered = true;
            if (!hoverPlayed && audioManager) {
                audioManager->playEffect("assets/audio/hover.wav");
                hoverPlayed = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                clicked = true;
                if (audioManager) {
                    audioManager->playEffect("assets/audio/click.wav");
                }
                return true;
            }
        } else {
            hovered = false;
            hoverPlayed = false;
        }
    }
    return false;
}

void Button::setClicked(bool clicked) {
    this->clicked = clicked;
}

void Button::clean() {
    if (normalTexture) {
        SDL_DestroyTexture(normalTexture);
        normalTexture = nullptr;
    }
    if (hoverTexture) {
        SDL_DestroyTexture(hoverTexture);
        hoverTexture = nullptr;
    }
    if (clickTexture) {
        SDL_DestroyTexture(clickTexture);
        clickTexture = nullptr;
    }
}

bool Button::isHovered(int x, int y) {
    // Sử dụng position để kiểm tra nếu con trỏ chuột nằm trong vùng của nút
    return x >= position.x && x <= position.x + position.w &&
           y >= position.y && y <= position.y + position.h;
}

bool Button::isClicked(int x, int y) {
    return isHovered(x, y);
}
