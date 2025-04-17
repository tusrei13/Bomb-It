#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include "buttons.h"
#include "menu.h" // For ScreenState
#include "include/SDL.h"
#include <vector>

class GameSettings {
public:
    GameSettings();
    ~GameSettings();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void clean();
    void reset(); // Phương thức để reset trạng thái các nút

private:
    SDL_Texture* backgroundTexture;
    Button onePlayerButton;
    Button twoPlayerButton;
    Button basketballButton;
    Button tombButton;
    Button easyButton;
    Button normalButton;
    Button hardButton;
    Button backButton;
    Button startButton;
    AudioManager* audioManager; // Thêm thành viên audioManager
};

#endif // GAME_SETTINGS_H
