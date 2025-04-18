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
    SDL_Renderer* renderer;  // Thêm biến renderer
    std::string selectedMap; // Thêm biến lưu map được chọn
    Game* game; // Con trỏ đến đối tượng Game
};

#endif // GAME_SETTINGS_H
