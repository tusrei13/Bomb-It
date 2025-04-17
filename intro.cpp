#include "intro.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include "AudioManager.h"
#include <iostream>

Intro::Intro()
    : introMusic(nullptr), currentImageIndex(0), lastImageTime(0), isFinished(false) {}

Intro::~Intro() {}

void Intro::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->audioManager = audioManager;
    audioManager->playMusic("assets/audio/intro.mp3", 0); // Play intro music once

    // Load intro images
    const std::vector<std::string> imagePaths = {
        "assets/image/intro/intro1.png",
        "assets/image/intro/intro2.png",
        "assets/image/intro/intro3.png"
    };

    for (const auto& path : imagePaths) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << path << " - " << IMG_GetError() << std::endl;
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (texture) {
            images.push_back(texture);
        }
    }

    lastImageTime = SDL_GetTicks(); // Initialize the timer
}

void Intro::handleEvents(SDL_Event& event, ScreenState& currentState) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        currentState = ScreenState::GAME_SETTINGS; // Transition to GAME_SETTINGS when Enter is pressed
    }
}

void Intro::update(ScreenState& currentState) {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastImageTime > 3000) { // 3000 ms = 3 seconds
        currentImageIndex++;
        lastImageTime = currentTime;

        if (currentImageIndex >= images.size()) {
            Mix_HaltMusic(); // Dừng âm thanh intro.mp3
            audioManager->playMusic("assets/audio/menu_music.mp3", -1); // Phát menu_music.mp3
            currentState = ScreenState::GAME_SETTINGS; // Chuyển sang GAME_SETTINGS
        }
    }
}

void Intro::render(SDL_Renderer* renderer) {
    if (currentImageIndex < images.size()) {
        SDL_RenderCopy(renderer, images[currentImageIndex], nullptr, nullptr); // Render current image
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
    // Reset trạng thái của phần INTRO
    currentImageIndex = 0;
    lastImageTime = SDL_GetTicks();
    isFinished = false;
    audioManager->playMusic("assets/audio/intro.mp3", 0); // Reset and play intro music
}