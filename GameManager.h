#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Renderer.h"
#include "AudioManager.h"

class GameManager {
public:
    GameManager();
    ~GameManager();

    bool init();
    void handleEvents();
    void update();
    void render();
    void clean();
    bool isRunning() const;

private:
    bool running;
    Renderer renderer;
    AudioManager audioManager;
};

#endif // GAMEMANAGER_H
