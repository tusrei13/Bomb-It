#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() : currentMusic(nullptr) {}

AudioManager::~AudioManager() {
    clean();
}

bool AudioManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_Volume(-1, MIX_MAX_VOLUME); // Set maximum volume for all channels
    std::cout << "AudioManager initialized successfully." << std::endl;
    return true;
}

void AudioManager::playMusic(const std::string& filePath, int loops) {
    if (currentMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    currentMusic = Mix_LoadMUS(filePath.c_str());
    if (!currentMusic) {
        std::cerr << "Failed to load music: " << filePath << " - " << Mix_GetError() << std::endl;
        return;
    }
    if (Mix_PlayMusic(currentMusic, loops) == -1) {
        std::cerr << "Failed to play music: " << filePath << " - " << Mix_GetError() << std::endl;
    }
}

void AudioManager::stopMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void AudioManager::playEffect(const std::string& filePath) {
    Mix_Chunk* effect = Mix_LoadWAV(filePath.c_str());
    if (!effect) {
        std::cerr << "Failed to load sound effect: " << filePath << " - " << Mix_GetError() << std::endl;
        return;
    }
    if (Mix_PlayChannel(-1, effect, 0) == -1) {
        std::cerr << "Failed to play sound effect: " << filePath << " - " << Mix_GetError() << std::endl;
    }
    Mix_FreeChunk(effect);
}

void AudioManager::playSound(const std::string& filePath) {
    Mix_Chunk* sound = Mix_LoadWAV(filePath.c_str());
    if (!sound) {
        std::cerr << "Failed to load sound: " << filePath << " - " << Mix_GetError() << std::endl;
        return;
    }
    Mix_PlayChannel(-1, sound, 0);
    Mix_FreeChunk(sound); // Giải phóng tài nguyên sau khi phát
}

void AudioManager::clean() {
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    Mix_CloseAudio();
    std::cout << "AudioManager cleaned up." << std::endl;
}
