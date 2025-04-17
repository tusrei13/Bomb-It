#ifndef MENU_H
#define MENU_H

#include <vector>
#include "buttons.h"
#include "include/SDL.h"

enum class ScreenState {
    MENU,
    HOW_TO_PLAY,
    INTRO,          // Add INTRO state
    GAME_SETTINGS,  // Add GAME_SETTINGS state
    GAME_MANAGER    // Add GAME_MANAGER state
};

class Menu {
public:
    Menu();
    ~Menu();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void clean();

private:
    SDL_Texture* backgroundTexture;
    Button startMenuButton;
    Button howToPlayButton;
};

#endif // MENU_H
