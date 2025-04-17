#ifndef BUTTONS_H
#define BUTTONS_H

#include <string>
#include "include/SDL.h"
#include "AudioManager.h"

class Button {
public:
    Button(const std::string& text, int x, int y, int width, int height, bool hasClickState = false);
    ~Button();

    void setTextures(SDL_Renderer* renderer, const std::string& normalPath, const std::string& hoverPath, const std::string& clickPath = "");
    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& event);
    void setClicked(bool clicked); // Add this method to set isClicked
    void setAudio(AudioManager* audioManager, const std::string& hoverSoundPath, const std::string& clickSoundPath);
    void resetClick(); // Add method to reset click state
    void clean();
    void setAudio(AudioManager* audioManager);

private:
    std::string text;
    int x, y, width, height;
    SDL_Texture* normalTexture;
    SDL_Texture* hoverTexture;
    SDL_Texture* clickTexture;
    bool isHovered;
    bool isClicked;
    bool hasClickState; // Indicates if the button has a click state
    AudioManager* audioManager;
    std::string hoverSoundPath;
    std::string clickSoundPath;
    bool hoverPlayed; // To prevent repeated hover sound
};

#endif // BUTTONS_H
