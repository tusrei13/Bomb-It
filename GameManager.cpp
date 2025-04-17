#include "GameManager.h"
#include <iostream>

GameManager::GameManager() : running(false) {}

GameManager::~GameManager() {}

bool GameManager::init() {
    // Initialize renderer and audio manager
    if (!renderer.init() || !audioManager.init()) {
        return false;
    }
    running = true;
    return true;
}

void GameManager::handleEvents() {
    // Handle input events
    // ...existing code...
}

void GameManager::update() {
    // Update game logic
    // ...existing code...
}

void GameManager::render() {
    renderer.render();
}

void GameManager::clean() {
    renderer.clean();
    audioManager.clean();
}

bool GameManager::isRunning() const {
    return running;
}