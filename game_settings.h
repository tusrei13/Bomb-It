#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include "buttons.h"
#include "menu.h"
#include "game.h"
#include "include/SDL.h"
#include <vector>
#include <string>

class GameSettings {
public:
    GameSettings(Game* game);
    ~GameSettings();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void clean();
    void reset(); // Phương thức để reset trạng thái các nút

private:
    SDL_Texture* backgroundTexture;
    Button basketballButton;
    Button tombButton;
    Button backButton;
    Button startButton;
    AudioManager* audioManager;
    SDL_Renderer* renderer;
    std::string selectedMap;
    std::string selectedMode;
    Game* game;
};

#endif // GAME_SETTINGS_H
