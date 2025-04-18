#include "game_settings.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>

GameSettings::GameSettings(Game* game)
    : backgroundTexture(nullptr),
      onePlayerButton("1 Player", 502, 302, 62, 30, true),
      twoPlayerButton("2 Player", 565, 302, 62, 30, true),
      basketballButton("Basketball", 502, 358, 218, 30, true),
      tombButton("Tomb", 721, 358, 218, 30, true),
      easyButton("Easy", 502, 414, 118, 30, true),
      normalButton("Normal", 621, 414, 118, 30, true),
      hardButton("Hard", 740, 414, 118, 30, true),
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

    onePlayerButton.setTextures(renderer, "assets/image/buttons/1player_normal.png", "assets/image/buttons/1player_hover.png", "assets/image/buttons/1player_click.png");
    twoPlayerButton.setTextures(renderer, "assets/image/buttons/2player_normal.png", "assets/image/buttons/2player_hover.png", "assets/image/buttons/2player_click.png");
    basketballButton.setTextures(renderer, "assets/image/buttons/basketball_normal.png", "assets/image/buttons/basketball_hover.png", "assets/image/buttons/basketball_click.png");
    tombButton.setTextures(renderer, "assets/image/buttons/tomb_normal.png", "assets/image/buttons/tomb_hover.png", "assets/image/buttons/tomb_click.png");
    easyButton.setTextures(renderer, "assets/image/buttons/easy_normal.png", "assets/image/buttons/easy_hover.png", "assets/image/buttons/easy_click.png");
    normalButton.setTextures(renderer, "assets/image/buttons/normal_normal.png", "assets/image/buttons/normal_hover.png", "assets/image/buttons/normal_click.png");
    hardButton.setTextures(renderer, "assets/image/buttons/hard_normal.png", "assets/image/buttons/hard_hover.png", "assets/image/buttons/hard_click.png");
    backButton.setTextures(renderer, "assets/image/buttons/game_settings_back_normal.png", "assets/image/buttons/game_settings_back_hover.png");
    startButton.setTextures(renderer, "assets/image/buttons/start_settings_button_normal.png", "assets/image/buttons/start_settings_button_hover.png");

    onePlayerButton.setAudio(audioManager);
    twoPlayerButton.setAudio(audioManager);
    basketballButton.setAudio(audioManager);
    tombButton.setAudio(audioManager);
    easyButton.setAudio(audioManager);
    normalButton.setAudio(audioManager);
    hardButton.setAudio(audioManager);
    backButton.setAudio(audioManager);
    startButton.setAudio(audioManager);

    if (audioManager) {
        audioManager->playMusic("assets/audio/menu_music.mp3");
    }
}

void GameSettings::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    onePlayerButton.render(renderer);
    twoPlayerButton.render(renderer);
    basketballButton.render(renderer);
    tombButton.render(renderer);
    easyButton.render(renderer);
    normalButton.render(renderer);
    hardButton.render(renderer);
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
            audioManager->playSound("assets/audio/click.mp3"); // Play click sound
            if (!selectedMap.empty() && game != nullptr) {
                game->init(this->renderer); // Sử dụng renderer đã lưu
                game->setCurrentMap(selectedMap);
                currentState = ScreenState::GAME;
            }
        }
    }

    // Handle player selection buttons
    if (onePlayerButton.handleEvent(event)) {
        twoPlayerButton.resetClick(); // Reset other button in the group
        std::cout << "1 Player selected!" << std::endl;
    }
    if (twoPlayerButton.handleEvent(event)) {
        onePlayerButton.resetClick(); // Reset other button in the group
        std::cout << "2 Player selected!" << std::endl;
    }

    // Handle arena selection buttons
    if (basketballButton.handleEvent(event)) {
        tombButton.resetClick(); // Reset other button in the group
        selectedMap = "basketball";
        std::cout << "Basketball arena selected!" << std::endl;
    }
    if (tombButton.handleEvent(event)) {
        basketballButton.resetClick(); // Reset other button in the group
        selectedMap = "tomb";
        std::cout << "Tomb arena selected!" << std::endl;
    }

    // Handle difficulty selection buttons
    if (easyButton.handleEvent(event)) {
        normalButton.resetClick(); // Reset other buttons in the group
        hardButton.resetClick();
        std::cout << "Easy difficulty selected!" << std::endl;
    }
    if (normalButton.handleEvent(event)) {
        easyButton.resetClick(); // Reset other buttons in the group
        hardButton.resetClick();
        std::cout << "Normal difficulty selected!" << std::endl;
    }
    if (hardButton.handleEvent(event)) {
        easyButton.resetClick(); // Reset other buttons in the group
        normalButton.resetClick();
        std::cout << "Hard difficulty selected!" << std::endl;
    }
}

void GameSettings::clean() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    onePlayerButton.clean();
    twoPlayerButton.clean();
    basketballButton.clean();
    tombButton.clean();
    easyButton.clean();
    normalButton.clean();
    hardButton.clean();
    backButton.clean();
    startButton.clean();
}

void GameSettings::reset() {
    backButton.resetClick(); // Reset trạng thái nút game_settings_back
    startButton.resetClick();   // Reset trạng thái nút start_settings_button
    easyButton.resetClick();            // Reset trạng thái nút easy
    normalButton.resetClick();          // Reset trạng thái nút normal
    hardButton.resetClick();            // Reset trạng thái nút hard
    basketballButton.resetClick();      // Reset trạng thái nút basketball
    tombButton.resetClick();            // Reset trạng thái nút tomb
    onePlayerButton.resetClick();       // Reset trạng thái nút 1player
    twoPlayerButton.resetClick();       // Reset trạng thái nút 2player
}