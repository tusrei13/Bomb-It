#include "intro.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include "AudioManager.h"
#include <iostream>

Intro::Intro() : introMusic(nullptr), currentImageIndex(0), lastImageTime(0), isFinished(false) {}

Intro::~Intro() {}

void Intro::init(SDL_Renderer* renderer) {
    // Load intro images
    std::vector<std::string> imagePaths = {
        "assets/image/intro/intro1.png",
        "assets/image/intro/intro2.png",
        "assets/image/intro/intro3.png"
    };

    for (const auto& path : imagePaths) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << std::endl;
            continue;
        }
        images.push_back(SDL_CreateTextureFromSurface(renderer, surface));
        SDL_FreeSurface(surface);
    }

    // Play intro music
    AudioManager audioManager;
    if (audioManager.init()) {
        audioManager.playMusic("assets/audio/intro.mp3", 1);
    }

    lastImageTime = SDL_GetTicks();
}

void Intro::handleEvents(SDL_Event& event, ScreenState& currentState) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        isFinished = true;
    }

    if (isFinished) {
        currentState = ScreenState::GAME_SETTINGS; // Transition to GAME_SETTINGS screen
    }
}

void Intro::update(ScreenState& currentState) {
    if (isFinished) return;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastImageTime > 2000) { // 2 seconds per image
        currentImageIndex++;
        lastImageTime = currentTime;

        if (currentImageIndex >= images.size()) {
            isFinished = true;
            currentState = ScreenState::GAME_SETTINGS; // Transition to GAME_SETTINGS
        }
    }
}

void Intro::render(SDL_Renderer* renderer) {
    if (currentImageIndex < images.size()) {
        SDL_RenderCopy(renderer, images[currentImageIndex], nullptr, nullptr);
    }
}

void Intro::clean() {
    for (auto texture : images) {
        SDL_DestroyTexture(texture);
    }
    images.clear();

    if (introMusic) {
        Mix_FreeMusic(introMusic);
        introMusic = nullptr;
    }
}

void Intro::reset() {
    currentImageIndex = 0;
    lastImageTime = SDL_GetTicks();
    isFinished = false;
}
