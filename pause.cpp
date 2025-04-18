#include "pause.h"
#include <SDL_image.h>
#include <iostream>

Pause::Pause()
    : pauseScreenTexture(nullptr),
      mainMenuButton("Main Menu", 403, 624, 198, 67),
      resumeButton("Resume", 688, 624, 198, 67),
      audioManager(nullptr),
      renderer(nullptr) {}

Pause::~Pause() {
    clean();
}

void Pause::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->renderer = renderer;
    this->audioManager = audioManager;

    SDL_Surface* surface = IMG_Load("assets/image/screens/pause.png");
    if (surface) {
        pauseScreenTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    mainMenuButton.setTextures(renderer,
        "assets/image/buttons/main_menu_button_normal.png",
        "assets/image/buttons/main_menu_button_hover.png");
    resumeButton.setTextures(renderer,
        "assets/image/buttons/resume_button_normal.png",
        "assets/image/buttons/resume_button_hover.png");

    mainMenuButton.setAudio(audioManager);
    resumeButton.setAudio(audioManager);
}

void Pause::handleEvents(SDL_Event& event, ScreenState& currentState, bool& isPaused) {
    if (mainMenuButton.handleEvent(event)) {
        if (event.type == SDL_MOUSEMOTION) {
            audioManager->playSound("assets/audio/hover.mp3");
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            audioManager->playSound("assets/audio/click.mp3");
            audioManager->stopMusic(); // Dừng nhạc game
            audioManager->playMusic("assets/audio/menu_music.mp3", -1); // Phát nhạc menu
            currentState = ScreenState::MENU;
            isPaused = false;
        }
    }
    if (resumeButton.handleEvent(event)) {
        if (event.type == SDL_MOUSEMOTION) {
            audioManager->playSound("assets/audio/hover.mp3");
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            audioManager->playSound("assets/audio/click.mp3");
            isPaused = false;
        }
    }
}

void Pause::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, pauseScreenTexture, nullptr, nullptr);
    mainMenuButton.render(renderer);
    resumeButton.render(renderer);
}

void Pause::clean() {
    if (pauseScreenTexture) {
        SDL_DestroyTexture(pauseScreenTexture);
        pauseScreenTexture = nullptr;
    }
    mainMenuButton.clean();
    resumeButton.clean();
}
