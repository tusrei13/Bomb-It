#include "game_settings.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>

GameSettings::GameSettings(Game* game)
    : backgroundTexture(nullptr),
      basketballButton("Basketball", 527, 344, 246, 41, true),
      tombButton("Tomb", 527, 410 , 246, 41, true),
      backButton("Back", 430, 625, 182, 64, false),
      startButton("Start", 662, 625, 192, 64, false),
      game(game) {}

GameSettings::~GameSettings() {}

void GameSettings::init(SDL_Renderer* renderer, AudioManager* audioManager) {
    this->renderer = renderer;  // Lưu renderer
    this->audioManager = audioManager;
    audioManager->playMusic("assets/audio/menu_music.mp3", -1); // Play menu music
    SDL_Surface* surface = IMG_Load("assets/image/screens/game_settings.png");
    if (!surface) {
        std::cerr << "Failed to load game_settings.png: " << IMG_GetError() << std::endl;
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    basketballButton.setTextures(renderer, "assets/image/buttons/basketball_normal.png", "assets/image/buttons/basketball_hover.png", "assets/image/buttons/basketball_click.png");
    tombButton.setTextures(renderer, "assets/image/buttons/tomb_normal.png", "assets/image/buttons/tomb_hover.png", "assets/image/buttons/tomb_click.png");
    backButton.setTextures(renderer, "assets/image/buttons/game_settings_back_normal.png", "assets/image/buttons/game_settings_back_hover.png");
    startButton.setTextures(renderer, "assets/image/buttons/start_settings_button_normal.png", "assets/image/buttons/start_settings_button_hover.png");

    basketballButton.setAudio(audioManager);
    tombButton.setAudio(audioManager);
    backButton.setAudio(audioManager);
    startButton.setAudio(audioManager);

    if (audioManager) {
        audioManager->playMusic("assets/audio/menu_music.mp3");
    }

}

void GameSettings::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    basketballButton.render(renderer);
    tombButton.render(renderer);
    backButton.render(renderer);
    startButton.render(renderer);
}

void GameSettings::handleEvents(SDL_Event& event, ScreenState& currentState) {
    if (backButton.handleEvent(event)) {
        if (event.type == SDL_MOUSEMOTION) {
            audioManager->playSound("assets/audio/hover.mp3"); // Play hover sound
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            audioManager->playSound("assets/audio/click.mp3"); // Play click sound
            audioManager->playMusic("assets/audio/menu_music.mp3", -1); // Play menu music again
            currentState = ScreenState::MENU; // Transition back to MENU
        }
    }

    if (startButton.handleEvent(event)) {
        if (event.type == SDL_MOUSEMOTION) {
            audioManager->playSound("assets/audio/hover.mp3"); // Play hover sound
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (!selectedMap.empty() && !selectedMode.empty()) {
                audioManager->playSound("assets/audio/click.mp3");
                audioManager->stopMusic();
                audioManager->playMusic("assets/audio/game_music.mp3", -1);
                game->init(this->renderer, this->audioManager);
                game->setCurrentMap(selectedMap);
                currentState = ScreenState::GAME;
            }
        }
    }

    // Handle map selection buttons
    if (basketballButton.handleEvent(event)) {
        tombButton.resetClick(); // Reset other button in the group
        selectedMap = "basketball";
        selectedMode = "arcade"; // Set default mode
        std::cout << "Basketball arena selected!" << std::endl;
    }
    if (tombButton.handleEvent(event)) {
        basketballButton.resetClick(); // Reset other button in the group
        selectedMap = "tomb";
        selectedMode = "arcade"; // Set default mode
        std::cout << "Tomb arena selected!" << std::endl;
    }
}

void GameSettings::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    basketballButton.clean();
    tombButton.clean();
    backButton.clean();
    startButton.clean();
}

void GameSettings::reset() {
    backButton.resetClick(); // Reset trạng thái nút game_settings_back
    startButton.resetClick();   // Reset trạng thái nút start_settings_button
    basketballButton.resetClick();      // Reset trạng thái nút basketball
    tombButton.resetClick();            // Reset trạng thái nút tomb
    selectedMap = "";
    selectedMode = "";
}