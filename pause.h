#ifndef PAUSE_H
#define PAUSE_H

#include "buttons.h"
#include "include/SDL.h"
#include "AudioManager.h"
#include "menu.h" // For ScreenState

class Pause {
public:
    Pause();
    ~Pause();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState, bool& isPaused);
    void clean();

private:
    SDL_Texture* backgroundTexture;
    Button mainMenuButton;
    Button resumeButton;
    AudioManager* audioManager;
};

#endif // PAUSE_H
