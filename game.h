#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "map.h"
#include "buttons.h"
#include "AudioManager.h"
#include "pause.h"
#include "player.h"
#include "menu.h"
#include "bomb.h"
#include "explosion.h" // Add this include
#include <memory>
#include <vector>

class Game {
public:
    Game();
    ~Game();

    void init(SDL_Renderer* renderer, AudioManager* audioManager);
    void setCurrentMap(const std::string& mapType);
    void handleEvents(SDL_Event& event, ScreenState& currentState);
    void update(); // thêm dòng này
    void render(SDL_Renderer* renderer);
    void clean();
    SDL_Texture* getBombTexture() const { return bombTexture; }

private:
    SDL_Texture* gameFrameTexture;
    SDL_Renderer* renderer;

    Map currentMap;
    Button pauseButton;
    Pause pause;
    AudioManager* audioManager;
    bool isPaused;

    Player player1; // Thêm nhân vật
    Player player2; // Thêm player2

    SDL_Texture* bombTexture;  // Texture cho bomb spritesheet
    std::vector<std::shared_ptr<Bomb>> bombs;
    std::vector<std::shared_ptr<Bomb>> bombsPlayer1;  // Bombs của player1
    std::vector<std::shared_ptr<Bomb>> bombsPlayer2;  // Bombs của player2
    bool player1CanPlaceBomb = true;  // Trạng thái đặt bomb của player1
    bool player2CanPlaceBomb = true;  // Trạng thái đặt bomb của player2
    void loadBombTexture(); // Phương thức load bomb spritesheet
    std::vector<std::unique_ptr<Explosion>> explosions; // Add this line
};
  
#endif
