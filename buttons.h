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
    bool isHovered(int x, int y); // Thêm phương thức kiểm tra hover
    bool isClicked(int x, int y); // Thêm phương thức kiểm tra click
    void setNormalPosition(int x, int y, int w, int h);
    void setHoverPosition(int x, int y, int w, int h);
    void setClickPosition(int x, int y, int w, int h);

private:
    std::string text;
    int x, y, width, height;
    SDL_Texture* normalTexture;
    SDL_Texture* hoverTexture;
    SDL_Texture* clickTexture;
    bool hovered; // Đổi tên từ isHovered
    bool clicked; // Đổi tên từ isClicked
    bool hasClickState; // Indicates if the button has a click state
    AudioManager* audioManager;
    std::string hoverSoundPath;
    std::string clickSoundPath;
    bool hoverPlayed; // To prevent repeated hover sound
    SDL_Rect position; // Thêm thành viên để lưu vị trí và kích thước của nút
    bool handleHover(const SDL_Event& event);
    SDL_Rect normalPosition;
    SDL_Rect hoverPosition;
    SDL_Rect clickPosition;
};

#endif // BUTTONS_H
