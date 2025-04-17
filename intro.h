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

    void init(SDL_Renderer* renderer, AudioManager* audioManager); // Cập nhật hàm init
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void update(ScreenState& currentState); // Update method now accepts ScreenState&
    void render(SDL_Renderer* renderer);
    void clean();
    void reset(); // Add this method to reset intro state

private:
    std::vector<SDL_Texture*> images; // Danh sách các ảnh intro
    int currentImageIndex = 0;        // Chỉ số ảnh hiện tại
    Uint32 lastImageTime = 0;         // Thời gian hiển thị ảnh cuối cùng
    bool isFinished = false;          // Trạng thái hoàn thành intro
    AudioManager* audioManager;       // Quản lý âm thanh
    Mix_Music* introMusic;            // Âm thanh intro
};

#endif // INTRO_H
