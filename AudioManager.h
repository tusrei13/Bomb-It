#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "include/SDL_mixer.h"
#include <string>
#include <map>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void playMusic(const std::string& filePath, int loops = -1);
    void stopMusic();
    void playEffect(const std::string& filePath);
    void playSound(const std::string& filePath);
    void clean();

private:
    Mix_Music* currentMusic;
    std::map<std::string, Mix_Chunk*> soundEffects; // Thêm map để cache sound effects
};

#endif // AUDIOMANAGER_H
