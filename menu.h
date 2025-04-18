#ifndef MENU_H
#define MENU_H

#include <vector>
#include "buttons.h"
#include "include/SDL.h"

// Forward declaration
class GameSettings;

enum class ScreenState {
    MENU,
    HOW_TO_PLAY,
    INTRO,
    GAME_SETTINGS,
    GAME_MANAGER,
    GAME // Thêm trạng thái GAME
};

class Menu {
public:
    Menu();
    ~Menu();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void clean();
    void setGameSettings(GameSettings* settings) { gameSettings = settings; }

private:
    SDL_Texture* backgroundTexture;
    Button startMenuButton;
    Button howToPlayButton;
    AudioManager* audioManager; // Thêm thành viên audioManager
    GameSettings* gameSettings;
};

#endif // MENU_H
