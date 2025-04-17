#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "include/SDL_mixer.h"
#include <string>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void playMusic(const std::string& filePath, int loops = -1);
    void stopMusic();
    void playEffect(const std::string& filePath);
    void clean();

private:
    Mix_Music* currentMusic;
};

#endif // AUDIOMANAGER_H
