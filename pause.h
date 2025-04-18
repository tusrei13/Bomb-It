#ifndef PAUSE_H
#define PAUSE_H

#include <SDL.h>
#include "buttons.h"
#include "AudioManager.h"
#include "menu.h"

class Pause {
public:
    Pause();
    ~Pause();
    
    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void handleEvents(SDL_Event& event, ScreenState& currentState, bool& isPaused);
    void render(SDL_Renderer* renderer);
    void clean();

private:
    SDL_Texture* pauseScreenTexture;
    Button mainMenuButton;
    Button resumeButton;
    AudioManager* audioManager;
    SDL_Renderer* renderer;
};

#endif
