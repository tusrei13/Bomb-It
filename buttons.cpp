#include "buttons.h"
#include "include/SDL_image.h"
#include <iostream>

Button::Button(const std::string& text, int x, int y, int width, int height, bool hasClickState)
    : text(text), x(x), y(y), width(width), height(height),
      normalTexture(nullptr), hoverTexture(nullptr), clickTexture(nullptr),
      hovered(false), clicked(false), hasClickState(hasClickState),
      audioManager(nullptr), hoverPlayed(false) {
    normalPosition = {x, y, width, height};
    hoverPosition = {x, y, width, height};  // Default same as normal
    clickPosition = {x, y, width, height};  // Default same as normal
    position = normalPosition;  // Current position starts as normal
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

void Button::setNormalPosition(int x, int y, int w, int h) {
    normalPosition = {x, y, w, h};
    if (!hovered && !clicked) {
        position = normalPosition;
    }
}

void Button::setHoverPosition(int x, int y, int w, int h) {
    hoverPosition = {x, y, w, h};
}

void Button::setClickPosition(int x, int y, int w, int h) {
    clickPosition = {x, y, w, h};
}

void Button::render(SDL_Renderer* renderer) {
    SDL_Rect* currentRect = &normalPosition;
    if (clicked && hasClickState) {
        currentRect = &clickPosition;
        SDL_RenderCopy(renderer, clickTexture, nullptr, currentRect);
    } else if (hovered) {
        currentRect = &hoverPosition;
        SDL_RenderCopy(renderer, hoverTexture, nullptr, currentRect);
    } else {
        SDL_RenderCopy(renderer, normalTexture, nullptr, currentRect);
    }
}

void Button::resetClick() {
    clicked = false;
}

bool Button::handleEvent(SDL_Event& event) {
    SDL_Point mousePos;
    switch(event.type) {
        case SDL_MOUSEMOTION:
            mousePos = {event.motion.x, event.motion.y};
            if (SDL_PointInRect(&mousePos, &normalPosition)) {
                if (!hovered && audioManager && !hoverPlayed) {
                    audioManager->playSound("assets/audio/hover.mp3");
                    hoverPlayed = true;
                }
                hovered = true;
                position = hoverPosition;
            } else {
                hovered = false;
                hoverPlayed = false;
                position = normalPosition;
            }
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            mousePos = {event.button.x, event.button.y};
            if (event.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&mousePos, &normalPosition)) {
                if (hasClickState) {
                    clicked = true;
                    position = clickPosition;
                }
                if (audioManager) {
                    audioManager->playSound("assets/audio/click.mp3"); 
                }
                return true;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            // Chỉ reset clicked state nếu button KHÔNG có clickState
            if (!hasClickState) {
                clicked = false;
            }
            break;
    }
    return false;
}

void Button::setClicked(bool isClicked) {
    // Chỉ set clicked state nếu button có clickState
    if (hasClickState) {
        clicked = isClicked;
    }
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
