#ifndef HOW_TO_PLAY_H
#define HOW_TO_PLAY_H

#include "buttons.h"
#include "include/SDL.h"
#include "menu.h" // Include for ScreenState

class HowToPlay {
public:
    HowToPlay();
    ~HowToPlay();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void render(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void clean();

private:
    SDL_Texture* backgroundTexture;
    Button backButton;
};

#endif // HOW_TO_PLAY_H
