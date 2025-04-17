#include "menu.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>

Menu::Menu()
    : backgroundTexture(nullptr),
      startMenuButton("Start", 701, 221, 287, 74),
      howToPlayButton("How to Play", 697, 327, 287, 74) {}

Menu::~Menu() {}

void Menu::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    // Load background image
    SDL_Surface* surface = IMG_Load("assets/image/screens/menu.png");
    if (!surface) {
        std::cerr << "Failed to load menu.png: " << IMG_GetError() << std::endl;
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Initialize buttons
    startMenuButton.setTextures(renderer, "assets/image/buttons/start_menu_button_normal.png", "assets/image/buttons/start_menu_button_hover.png");
    howToPlayButton.setTextures(renderer, "assets/image/buttons/how_to_play_button_normal.png", "assets/image/buttons/how_to_play_button_hover.png");

    if (!audioManager->init()) {
        std::cerr << "Failed to initialize AudioManager for menu buttons!" << std::endl;
    }

    startMenuButton.setAudio(audioManager);
    howToPlayButton.setAudio(audioManager);
}

void Menu::render(SDL_Renderer* renderer) {
    // Render background
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    // Render buttons
    startMenuButton.render(renderer);
    howToPlayButton.render(renderer);
}

void Menu::handleEvents(SDL_Event& event, ScreenState& currentState) {
    // Handle button events
    if (startMenuButton.handleEvent(event)) {
        std::cout << "Start button clicked! Transitioning to Intro..." << std::endl;
        currentState = ScreenState::INTRO; // Fix INTRO state
    }
    if (howToPlayButton.handleEvent(event)) {
        std::cout << "How to Play button clicked!" << std::endl;
        currentState = ScreenState::HOW_TO_PLAY; // Transition to HOW TO PLAY screen
    }
}

void Menu::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    startMenuButton.clean();
    howToPlayButton.clean();
}
