#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "map.h"
#include "buttons.h"
#include "AudioManager.h"
#include "menu.h"
#include "pause.h"

class Game {
public:
    Game();
    ~Game();
    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);  // Update this line
    void setCurrentMap(const std::string& mapType);
    void clean();

private:
    Map currentMap;
    SDL_Texture* gameFrameTexture;
    SDL_Renderer* renderer;
    Button pauseButton;
    AudioManager* audioManager;
    bool isPaused;
    Pause pause;
};

#endif
