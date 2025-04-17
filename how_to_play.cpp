#include "how_to_play.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>

HowToPlay::HowToPlay()
    : backgroundTexture(nullptr),
      backButton("Back", 42, 34, 182, 64) {}

HowToPlay::~HowToPlay() {}

void HowToPlay::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->audioManager = audioManager;
    audioManager->playMusic("assets/audio/menu_music.mp3", -1); // Play menu music
    // Load background image
    SDL_Surface* surface = IMG_Load("assets/image/screens/how_to_play.png");
    if (!surface) {
        std::cerr << "Failed to load how_to_play.png: " << IMG_GetError() << std::endl;
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Initialize back button
    if (!audioManager->init()) {
        std::cerr << "Failed to initialize AudioManager for how-to-play button!" << std::endl;
    }

    backButton.setTextures(renderer, "assets/image/buttons/how_to_play_back_normal.png", "assets/image/buttons/how_to_play_back_hover.png");
    backButton.setAudio(audioManager);

    if (audioManager) {
        audioManager->playMusic("assets/audio/menu_music.mp3");
    }
}

void HowToPlay::render(SDL_Renderer* renderer) {
    // Render background
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    // Render back button
    backButton.render(renderer);
}

void HowToPlay::handleEvents(SDL_Event& event, ScreenState& currentState) {
    if (backButton.handleEvent(event)) {
        if (event.type == SDL_MOUSEMOTION) {
            audioManager->playSound("assets/audio/hover.wav"); // Play hover sound
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            audioManager->playSound("assets/audio/click.wav"); // Play click sound
            currentState = ScreenState::MENU; // Transition back to MENU screen
        }
    }
}

void HowToPlay::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    backButton.clean();
}
