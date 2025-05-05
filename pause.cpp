#include "pause.h"
#include "include/SDL_image.h"
#include <iostream>

Pause::Pause()
    : backgroundTexture(nullptr),
      mainMenuButton("Main Menu", 403, 624, 198, 67),
      resumeButton("Resume", 688, 624, 198, 67),
      audioManager(nullptr) {}

Pause::~Pause() {
    clean();
}

void Pause::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->audioManager = audioManager;

    // Load background
    SDL_Surface* surface = IMG_Load("assets/image/screens/pause.png");
    if (!surface) {
        std::cerr << "Failed to load pause.png: " << IMG_GetError() << std::endl;
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Initialize buttons
    mainMenuButton.setTextures(renderer, 
        "assets/image/buttons/main_menu_button_normal.png",
        "assets/image/buttons/main_menu_button_hover.png");
    
    resumeButton.setTextures(renderer,
        "assets/image/buttons/resume_button_normal.png", 
        "assets/image/buttons/resume_button_hover.png");

    // Set audio for buttons
    mainMenuButton.setAudio(audioManager);
    resumeButton.setAudio(audioManager);
}

void Pause::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    mainMenuButton.render(renderer);
    resumeButton.render(renderer);
}

void Pause::handleEvents(SDL_Event& event, ScreenState& currentState, bool& isPaused) {
    if (mainMenuButton.handleEvent(event)) {
        if (event.type == SDL_MOUSEMOTION) {
            audioManager->playSound("assets/audio/hover.mp3");
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            audioManager->playSound("assets/audio/click.mp3");
            audioManager->playMusic("assets/audio/menu_music.mp3", -1);
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

void Pause::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    mainMenuButton.clean();
    resumeButton.clean();
}
