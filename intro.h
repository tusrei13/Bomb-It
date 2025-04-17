#ifndef INTRO_H
#define INTRO_H

#include "menu.h" // For ScreenState
#include "include/SDL.h"
#include "include/SDL_mixer.h"
#include <vector>
#include <string>

class Intro {
public:
    Intro();
    ~Intro();

    void init(SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void update(ScreenState& currentState); // Update method now accepts ScreenState&
    void render(SDL_Renderer* renderer);
    void clean();
    void reset(); // Add this method to reset intro state

private:
    std::vector<SDL_Texture*> images;
    Mix_Music* introMusic;
    int currentImageIndex;
    Uint32 lastImageTime;
    bool isFinished;
};

#endif // INTRO_H
