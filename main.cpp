#include "include/SDL.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int TILE_SIZE = 47;
const int MAP_WIDTH = 17;
const int MAP_HEIGHT = 15;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

enum GameState {
    MENU,
    HOW_TO_PLAY,
    GAME_SETTINGS,
    PLAYING,
    INTRO_SEQUENCE,
    PAUSE, // Add new game state for pause screen
    YOU_WIN, // Add new game state for win screen
    YOU_LOSE // Add new game state for lose screen
};

enum Difficulty {
    EASY,
    NORMAL,
    HARD
};

struct Player {
    SDL_Texture* texture;
    SDL_Rect rect;
    int x, y;
    // Removed attributes: alive, bombCount, explosionRange, speed, lives, shield, invincible
};

struct Bomb {
    SDL_Texture* texture;
    SDL_Rect rect;
    int x, y;
    Uint32 startTime;
    bool exploded;
};

struct Bot : public Player {
    void update(const std::vector<std::vector<int>>& map, const std::vector<Bomb>& bombs, const Player& target) {
        // Simple AI logic to chase the target player and avoid bombs
        // Removed logic related to alive
        for (const auto& bomb : bombs) {
            if (!bomb.exploded && abs(x - bomb.x) <= 1 && abs(y - bomb.y) <= 1) {
                if (x < bomb.x) x--;
                else if (x > bomb.x) x++;
                if (y < bomb.y) y--;
                else if (y > bomb.y) y++;
                return;
            }
        }

        // Chase the target player
        if (x < target.x) x++;
        else if (x > target.x) x--;
        if (y < target.y) y++;
        else if (y > target.y) y--;
    }

    bool shouldPlaceBomb(const std::vector<std::vector<int>>& map, const Player& target) {
        // Simple logic to place a bomb if the target is within range
        return abs(x - target.x) <= 1 && abs(y - target.y) <= 1;
    }
};

struct GameSettings {
    int players;
    std::string arena;
    Difficulty difficulty;
};

void loadMapFromTxt(const std::string& filename, std::vector<std::vector<int>>& map) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << filename << std::endl;
        return;
    }

    map.resize(MAP_HEIGHT, std::vector<int>(MAP_WIDTH));
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            file >> map[y][x];
        }
    }
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << path << " " << IMG_GetError() << std::endl;
    }
    return texture;
}

void handlePlayerDeath(Player& player) {
    // Removed logic related to shield, invincible, and alive
    player.texture = nullptr; // Set texture to nullptr to indicate death
}

void drawMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map) {
    SDL_Rect destRect = { 350, 0, TILE_SIZE, TILE_SIZE }; // Start rendering from x = 350
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            destRect.x = 350 + x * TILE_SIZE; // Offset by 350 pixels from the left
            destRect.y = y * TILE_SIZE;

            // Ensure the map does not exceed the right boundary (WINDOW_WIDTH - 103)
            if (destRect.x + TILE_SIZE <= WINDOW_WIDTH - 103) {
                SDL_RenderFillRect(renderer, &destRect); // Render as a filled rectangle
            }
        }
    }
}

void handleBombExplosion(Bomb& bomb, std::vector<Player>& players, std::vector<Bot>& bots, std::vector<std::vector<int>>& map, Mix_Chunk* explosionSound, SDL_Texture* floorTexture) {
    if (SDL_GetTicks() - bomb.startTime >= 3000 && !bomb.exploded) {
        bomb.exploded = true;
        Mix_PlayChannel(-1, explosionSound, 0);

        // Check for player collision with explosion
        for (auto& player : players) {
            if (player.x == bomb.x && player.y == bomb.y) {
                handlePlayerDeath(player);
            }
        }

        // Check for bot collision with explosion
        for (auto& bot : bots) {
            if (bot.x == bomb.x && bot.y == bomb.y) {
                handlePlayerDeath(bot);
            }
        }

        // Destroy bricks
        if (map[bomb.y][bomb.x] == 1) { // Assuming 1 represents a brick
            map[bomb.y][bomb.x] = 0; // Assuming 0 represents a floor
        }
    }
}

void drawMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* startMenuButtonTextures[2], SDL_Texture* howToPlayButtonTextures[2], bool startHover, bool howToPlayHover) {
    SDL_RenderCopy(renderer, menuTexture, nullptr, nullptr);

    SDL_Rect startButtonRect = { 701, 221, 287, 74 }; // Updated position and size for START button
    SDL_RenderCopy(renderer, startMenuButtonTextures[startHover ? 1 : 0], nullptr, &startButtonRect);

    SDL_Rect howToPlayButtonRect = { 697, 327, 287, 74 }; // Updated position and size for HOW TO PLAY button
    SDL_RenderCopy(renderer, howToPlayButtonTextures[howToPlayHover ? 1 : 0], nullptr, &howToPlayButtonRect);
}

void drawHowToPlay(SDL_Renderer* renderer, SDL_Texture* howToPlayTexture, SDL_Texture* howToPlayBackButtonTextures[2], bool backHover) {
    SDL_RenderCopy(renderer, howToPlayTexture, nullptr, nullptr);

    SDL_Rect backButtonRect = { 42, 34, 182, 64 }; // Position and size for BACK button in HOW TO PLAY
    SDL_RenderCopy(renderer, howToPlayBackButtonTextures[backHover ? 1 : 0], nullptr, &backButtonRect);
}

void drawGameSettings(SDL_Renderer* renderer, SDL_Texture* gameSettingsTexture, SDL_Texture* gameSettingsBackButtonTextures[2], SDL_Texture* startSettingsButtonTextures[2], bool backHover, bool startHover, const GameSettings& settings, SDL_Texture* player1Textures[3], SDL_Texture* player2Textures[3], SDL_Texture* basketballTextures[3], SDL_Texture* tombTextures[3], SDL_Texture* easyTextures[3], SDL_Texture* normalTextures[3], SDL_Texture* hardTextures[3], int selectedPlayer, int selectedArena, int selectedDifficulty, bool player1Hover, bool player2Hover, bool basketballHover, bool tombHover, bool easyHover, bool normalHover, bool hardHover) {
    SDL_RenderCopy(renderer, gameSettingsTexture, nullptr, nullptr);

    SDL_Rect backButtonRect = { 430, 625, 182, 64 }; // Position and size for BACK button in GAME SETTINGS
    SDL_RenderCopy(renderer, gameSettingsBackButtonTextures[backHover ? 1 : 0], nullptr, &backButtonRect);

    SDL_Rect startButtonRect = { 662, 625, 192, 64 }; // Position and size for START button
    SDL_RenderCopy(renderer, startSettingsButtonTextures[startHover ? 1 : 0], nullptr, &startButtonRect);

    // Draw player buttons
    SDL_Rect player1Rect = { 502, 302, 62, 30 };
    SDL_Rect player2Rect = { 565, 302, 62, 30 };
    SDL_RenderCopy(renderer, player1Textures[selectedPlayer == 1 ? 2 : (player1Hover ? 1 : 0)], nullptr, &player1Rect);
    SDL_RenderCopy(renderer, player2Textures[selectedPlayer == 2 ? 2 : (player2Hover ? 1 : 0)], nullptr, &player2Rect);

    // Draw arena buttons
    SDL_Rect basketballRect = { 502, 358, 218, 30 };
    SDL_Rect tombRect = { 721, 358, 218, 30 };
    SDL_RenderCopy(renderer, basketballTextures[selectedArena == 1 ? 2 : (basketballHover ? 1 : 0)], nullptr, &basketballRect);
    SDL_RenderCopy(renderer, tombTextures[selectedArena == 2 ? 2 : (tombHover ? 1 : 0)], nullptr, &tombRect);

    // Draw difficulty buttons
    SDL_Rect easyRect = { 502, 414, 118, 30 };
    SDL_Rect normalRect = { 621, 414, 118, 30 };
    SDL_Rect hardRect = { 740, 414, 118, 30 };
    SDL_RenderCopy(renderer, easyTextures[selectedDifficulty == EASY ? 2 : (easyHover ? 1 : 0)], nullptr, &easyRect);
    SDL_RenderCopy(renderer, normalTextures[selectedDifficulty == NORMAL ? 2 : (normalHover ? 1 : 0)], nullptr, &normalRect);
    SDL_RenderCopy(renderer, hardTextures[selectedDifficulty == HARD ? 2 : (hardHover ? 1 : 0)], nullptr, &hardRect);
}

void playIntroSequence(SDL_Renderer* renderer, SDL_Texture* introTextures[3]) {
    for (int i = 0; i < 3; ++i) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, introTextures[i], nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(2000); // Display each image for 2 seconds
    }
}

void updateGameSettings(GameSettings& settings, std::vector<Bot>& bots) {
    if (settings.players == 1) {
        bots.resize(3); // 1 player mode includes 3 AI bots
    } else if (settings.players == 2) {
        bots.resize(2); // 2 player mode includes 2 AI bots
    }

    // Removed logic for setting bot.speed based on difficulty
}

void drawPauseScreen(SDL_Renderer* renderer, SDL_Texture* pauseTexture, SDL_Texture* mainMenuButtonTextures[2], SDL_Texture* resumeButtonTextures[2], bool mainMenuHover, bool resumeHover) {
    SDL_RenderCopy(renderer, pauseTexture, nullptr, nullptr);

    SDL_Rect mainMenuButtonRect = { 400, 200, 300, 100 };
    SDL_RenderCopy(renderer, mainMenuButtonTextures[mainMenuHover ? 1 : 0], nullptr, &mainMenuButtonRect);

    SDL_Rect resumeButtonRect = { 400, 350, 300, 100 };
    SDL_RenderCopy(renderer, resumeButtonTextures[resumeHover ? 1 : 0], nullptr, &resumeButtonRect);
}

void drawWinScreen(SDL_Renderer* renderer, SDL_Texture* winTexture) {
    SDL_RenderCopy(renderer, winTexture, nullptr, nullptr);
}

void drawLoseScreen(SDL_Renderer* renderer, SDL_Texture* loseTexture) {
    SDL_RenderCopy(renderer, loseTexture, nullptr, nullptr);
}

void renderGameScreen(SDL_Renderer* renderer, SDL_Texture* leftPanelTexture, SDL_Texture* topFrameTexture, SDL_Texture* bottomFrameTexture, SDL_Texture* rightFrameTexture, SDL_Texture* mapTexture) {
    // Render left panel
    SDL_Rect leftPanelRect = { 0, 0, 365, 720 };
    SDL_RenderCopy(renderer, leftPanelTexture, nullptr, &leftPanelRect);

    // Render top border (frame)
    SDL_Rect topFrameRect = { 365, 0, 799, 3 };
    SDL_RenderCopy(renderer, topFrameTexture, nullptr, &topFrameRect);

    // Render game map grid
    SDL_Rect mapRect = { 365, 3, 799, 705 };
    SDL_RenderCopy(renderer, mapTexture, nullptr, &mapRect);

    // Render bottom border (frame)
    SDL_Rect bottomFrameRect = { 365, 708, 799, 12 };
    SDL_RenderCopy(renderer, bottomFrameTexture, nullptr, &bottomFrameRect);

    // Render right border (frame)
    SDL_Rect rightFrameRect = { 1164, 0, 116, 720 };
    SDL_RenderCopy(renderer, rightFrameTexture, nullptr, &rightFrameRect);
}

void renderMapFromTxt(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* textures[]) {
    SDL_Rect destRect = { 365, 3, TILE_SIZE, TILE_SIZE }; // Start rendering from x = 365, y = 3
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            destRect.x = 365 + x * TILE_SIZE;
            destRect.y = 3 + y * TILE_SIZE;

            int tileId = map[y][x];
            if (tileId >= 0 && tileId <= 5) {
                SDL_RenderCopy(renderer, textures[tileId], nullptr, &destRect);
            }
        }
    }
}

Mix_Chunk* clickSound = nullptr;

Mix_Music* menuMusic = nullptr;
Mix_Music* gameMusic = nullptr;
Mix_Music* winMusic = nullptr;
Mix_Music* loseMusic = nullptr;

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Bomb It", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Load textures
    SDL_Texture* playerTextures[4] = {
        loadTexture(renderer, "assets/image/players/player1.png"),
        loadTexture(renderer, "assets/image/players/player2.png"),
        loadTexture(renderer, "assets/image/players/player3.png"),
        loadTexture(renderer, "assets/image/players/player4.png")
    };

    SDL_Texture* leftPanelTexture = loadTexture(renderer, "assets/image/screens/left_panel.png");
    SDL_Texture* topFrameTexture = loadTexture(renderer, "assets/image/screens/frame_top.png");
    SDL_Texture* bottomFrameTexture = loadTexture(renderer, "assets/image/screens/frame_bottom.png");
    SDL_Texture* rightFrameTexture = loadTexture(renderer, "assets/image/screens/frame_right.png");

    SDL_Texture* basketballTextures[6] = {
        loadTexture(renderer, "assets/image/tiles/basketball/floor.png"),
        loadTexture(renderer, "assets/image/tiles/basketball/brick_1.png"),
        loadTexture(renderer, "assets/image/tiles/basketball/wall_1.png"),
        loadTexture(renderer, "assets/image/tiles/basketball/wall_2.png"),
        loadTexture(renderer, "assets/image/tiles/basketball/brick_2.png"),
        loadTexture(renderer, "assets/image/tiles/basketball/brick_3.png")
    };

    SDL_Texture* tombTextures[6] = {
        loadTexture(renderer, "assets/image/tiles/tomb/floor.png"),
        loadTexture(renderer, "assets/image/tiles/tomb/brick_1.png"),
        loadTexture(renderer, "assets/image/tiles/tomb/wall_1.png"),
        loadTexture(renderer, "assets/image/tiles/tomb/wall_2.png"),
        loadTexture(renderer, "assets/image/tiles/tomb/brick_2.png"),
        loadTexture(renderer, "assets/image/tiles/tomb/brick_3.png")
    };

    if (!leftPanelTexture || !topFrameTexture || !bottomFrameTexture || !rightFrameTexture) {
        std::cerr << "Failed to load one or more textures. Exiting..." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Load menu texture with error handling
    SDL_Texture* menuTexture = loadTexture(renderer, "assets/image/screens/menu.png");
    if (!menuTexture) {
        std::cerr << "Failed to load menu texture. Exiting..." << std::endl;
        return -1;
    }

    SDL_Texture* startMenuButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/start_menu_button_normal.png"),
        loadTexture(renderer, "assets/image/buttons/start_menu_button_hover.png")
    };
    if (!startMenuButtonTextures[0] || !startMenuButtonTextures[1]) {
        std::cerr << "Failed to load start menu button textures. Exiting..." << std::endl;
        return -1;
    }

    SDL_Texture* howToPlayButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/how_to_play_button_normal.png"),
        loadTexture(renderer, "assets/image/buttons/how_to_play_button_hover.png")
    };
    if (!howToPlayButtonTextures[0] || !howToPlayButtonTextures[1]) {
        std::cerr << "Failed to load how-to-play button textures. Exiting..." << std::endl;
        return -1;
    }

    SDL_Texture* howToPlayTexture = loadTexture(renderer, "assets/image/screens/how_to_play.png");
    SDL_Texture* howToPlayBackButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/how_to_play_back_normal.png"),
        loadTexture(renderer, "assets/image/buttons/how_to_play_back_hover.png")
    };
    SDL_Texture* gameSettingsTexture = loadTexture(renderer, "assets/image/screens/game_settings.png");
    SDL_Texture* startSettingsButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/start_settings_button_normal.png"),
        loadTexture(renderer, "assets/image/buttons/start_settings_button_hover.png")
    };

    // Load intro textures
    SDL_Texture* introTextures[3] = {
        loadTexture(renderer, "assets/image/intro/intro1.png"),
        loadTexture(renderer, "assets/image/intro/intro2.png"),
        loadTexture(renderer, "assets/image/intro/intro3.png")
    };

    // Load player button textures
    SDL_Texture* player1Textures[3] = {
        loadTexture(renderer, "assets/image/buttons/player1_normal.png"),
        loadTexture(renderer, "assets/image/buttons/player1_hover.png"),
        loadTexture(renderer, "assets/image/buttons/player1_click.png")
    };
    SDL_Texture* player2Textures[3] = {
        loadTexture(renderer, "assets/image/buttons/player2_normal.png"),
        loadTexture(renderer, "assets/image/buttons/player2_hover.png"),
        loadTexture(renderer, "assets/image/buttons/player2_click.png")
    };

    // Load arena button textures
    SDL_Texture* basketballArenaTextures[3] = {
        loadTexture(renderer, "assets/image/buttons/basketball_normal.png"),
        loadTexture(renderer, "assets/image/buttons/basketball_hover.png"),
        loadTexture(renderer, "assets/image/buttons/basketball_click.png")
    };
    SDL_Texture* tombArenaTextures[3] = {
        loadTexture(renderer, "assets/image/buttons/tomb_normal.png"),
        loadTexture(renderer, "assets/image/buttons/tomb_hover.png"),
        loadTexture(renderer, "assets/image/buttons/tomb_click.png")
    };

    // Load difficulty button textures
    SDL_Texture* easyTextures[3] = {
        loadTexture(renderer, "assets/image/buttons/easy_normal.png"),
        loadTexture(renderer, "assets/image/buttons/easy_hover.png"),
        loadTexture(renderer, "assets/image/buttons/easy_click.png")
    };
    SDL_Texture* normalTextures[3] = {
        loadTexture(renderer, "assets/image/buttons/normal_normal.png"),
        loadTexture(renderer, "assets/image/buttons/normal_hover.png"),
        loadTexture(renderer, "assets/image/buttons/normal_click.png")
    };
    SDL_Texture* hardTextures[3] = {
        loadTexture(renderer, "assets/image/buttons/hard_normal.png"),
        loadTexture(renderer, "assets/image/buttons/hard_hover.png"),
        loadTexture(renderer, "assets/image/buttons/hard_click.png")
    };

    SDL_Texture* gameSettingsBackButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/game_settings_back_normal.png"),
        loadTexture(renderer, "assets/image/buttons/game_settings_back_hover.png")
    };

    SDL_Texture* pauseButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/pause_button_normal.png"),
        loadTexture(renderer, "assets/image/buttons/pause_button_hover.png")
    };

    // Load sound
    Mix_Chunk* explosionSound = Mix_LoadWAV("assets/sound/explosion.mp3");
    if (!explosionSound) {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load click sound
    clickSound = Mix_LoadWAV("assets/sound/click.mp3");
    if (!clickSound) {
        std::cerr << "Failed to load click sound: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load background music
    menuMusic = Mix_LoadMUS("assets/sound/menu_music.mp3");
    if (!menuMusic) {
        std::cerr << "Failed to load menu music: " << Mix_GetError() << std::endl;
        return -1;
    }

    gameMusic = Mix_LoadMUS("assets/sound/game_music.mp3");
    if (!gameMusic) {
        std::cerr << "Failed to load game music: " << Mix_GetError() << std::endl;
        return -1;
    }

    winMusic = Mix_LoadMUS("assets/sound/win_music.mp3");
    if (!winMusic) {
        std::cerr << "Failed to load win music: " << Mix_GetError() << std::endl;
        return -1;
    }

    loseMusic = Mix_LoadMUS("assets/sound/lose_music.mp3");
    if (!loseMusic) {
        std::cerr << "Failed to load lose music: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Play menu music initially
    Mix_PlayMusic(menuMusic, -1);

    // Load maps from .txt files
    std::vector<std::vector<int>> basketballMap, tombMap;
    loadMapFromTxt("map/basketball_map.txt", basketballMap);
    loadMapFromTxt("map/tomb_map.txt", tombMap);

    // Initialize players and bots
    std::vector<Player> players(2);
    std::vector<Bot> bots;
    for (int i = 0; i < 2; ++i) {
        players[i].texture = playerTextures[i];
        players[i].rect = { 0, 0, TILE_SIZE, TILE_SIZE };
        players[i].x = 0;
        players[i].y = 0;
    }

    for (int i = 0; i < 3; ++i) { // Example: 3 bots
        bots.emplace_back();
        bots[i].texture = playerTextures[i + 2];
        bots[i].rect = { 0, 0, TILE_SIZE, TILE_SIZE };
        bots[i].x = (i % 2 == 0) ? 0 : MAP_WIDTH - 1;
        bots[i].y = MAP_HEIGHT - 1;
    }

    GameSettings settings = {1, "basketball", EASY};

    // Update game settings based on selected mode
    updateGameSettings(settings, bots);

    // Set initial positions for players and bots
    players[0].x = 0; players[0].y = 0;
    players[1].x = MAP_WIDTH - 1; players[1].y = 0;

    std::vector<Bomb> bombs;

    int selectedPlayer = 1;
    int selectedArena = 1;
    int selectedDifficulty = EASY;

    SDL_Texture* mainMenuButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/main_menu_button_normal.png"),
        loadTexture(renderer, "assets/image/buttons/main_menu_button_hover.png")
    };
    SDL_Texture* resumeButtonTextures[2] = {
        loadTexture(renderer, "assets/image/buttons/resume_button_normal.png"),
        loadTexture(renderer, "assets/image/buttons/resume_button_hover.png")
    };

    SDL_Texture* winTexture = loadTexture(renderer, "assets/image/you_win.png");
    SDL_Texture* loseTexture = loadTexture(renderer, "assets/image/you_lose.png");

    GameState gameState = MENU;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (gameState == MENU) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Correct hover detection for START and HOW TO PLAY buttons
                bool startHover = (x >= 701 && x <= 988 && y >= 221 && y <= 295); // Updated to match START button position and size
                bool howToPlayHover = (x >= 697 && x <= 984 && y >= 327 && y <= 401); // Updated to match HOW TO PLAY button position and size

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (startHover || howToPlayHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    }
                    if (startHover) {
                        gameState = GAME_SETTINGS; // Transition to GAME_SETTINGS
                    } else if (howToPlayHover) {
                        gameState = HOW_TO_PLAY; // Transition to HOW_TO_PLAY
                    }
                }

                // Clear screen before rendering
                SDL_RenderClear(renderer);

                // Render menu
                drawMenu(renderer, menuTexture, startMenuButtonTextures, howToPlayButtonTextures, startHover, howToPlayHover);

                // Present the rendered frame
                SDL_RenderPresent(renderer);
            } else if (gameState == INTRO_SEQUENCE) {
                playIntroSequence(renderer, introTextures);
                gameState = GAME_SETTINGS;
            } else if (gameState == HOW_TO_PLAY) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool backHover = (x >= 42 && x <= 224 && y >= 34 && y <= 98); // Position for BACK button in HOW TO PLAY

                if (event.type == SDL_MOUSEBUTTONDOWN && backHover) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    gameState = MENU; // Return to MENU
                }

                // Clear screen before rendering
                SDL_RenderClear(renderer);

                // Render HOW TO PLAY screen
                drawHowToPlay(renderer, howToPlayTexture, howToPlayBackButtonTextures, backHover);

                // Present the rendered frame
                SDL_RenderPresent(renderer);
            } else if (gameState == GAME_SETTINGS) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Hover detection for buttons
                bool backHover = (x >= 430 && x <= 612 && y >= 625 && y <= 689);
                bool startHover = (x >= 662 && x <= 854 && y >= 625 && y <= 689);
                bool player1Hover = (x >= 502 && x <= 564 && y >= 302 && y <= 332);
                bool player2Hover = (x >= 565 && x <= 627 && y >= 302 && y <= 332);
                bool basketballHover = (x >= 502 && x <= 720 && y >= 358 && y <= 388);
                bool tombHover = (x >= 721 && x <= 939 && y >= 358 && y <= 388);
                bool easyHover = (x >= 502 && x <= 620 && y >= 414 && y <= 444);
                bool normalHover = (x >= 621 && x <= 739 && y >= 414 && y <= 444);
                bool hardHover = (x >= 740 && x <= 858 && y >= 414 && y <= 444);

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (backHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        gameState = MENU; // Return to MENU
                    } else if (startHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound

                        // Play game music when transitioning to PLAYING
                        Mix_HaltMusic();
                        Mix_PlayMusic(gameMusic, -1);

                        gameState = PLAYING; // Start the game
                    } else if (player1Hover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedPlayer = 1; // Select player1
                    } else if (player2Hover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedPlayer = 2; // Select player2
                    } else if (basketballHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedArena = 1; // Select basketball arena
                    } else if (tombHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedArena = 2; // Select tomb arena
                    } else if (easyHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedDifficulty = EASY; // Select easy difficulty
                    } else if (normalHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedDifficulty = NORMAL; // Select normal difficulty
                    } else if (hardHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        selectedDifficulty = HARD; // Select hard difficulty
                    }
                }

                // Clear screen before rendering
                SDL_RenderClear(renderer);

                // Render GAME SETTINGS screen
                drawGameSettings(renderer, gameSettingsTexture, gameSettingsBackButtonTextures, startSettingsButtonTextures, backHover, startHover, settings, player1Textures, player2Textures, basketballArenaTextures, tombArenaTextures, easyTextures, normalTextures, hardTextures, selectedPlayer, selectedArena, selectedDifficulty, player1Hover, player2Hover, basketballHover, tombHover, easyHover, normalHover, hardHover);

                // Present the rendered frame
                SDL_RenderPresent(renderer);
            } else if (gameState == PLAYING) {
                // Clear screen before rendering
                SDL_RenderClear(renderer);

                // Render game screen based on the selected arena
                if (selectedArena == 1) {
                    renderGameScreen(renderer, leftPanelTexture, topFrameTexture, bottomFrameTexture, rightFrameTexture, nullptr);
                    renderMapFromTxt(renderer, basketballMap, basketballTextures);
                } else if (selectedArena == 2) {
                    renderGameScreen(renderer, leftPanelTexture, topFrameTexture, bottomFrameTexture, rightFrameTexture, nullptr);
                    renderMapFromTxt(renderer, tombMap, tombTextures);
                }

                // Draw Pause button with updated position and size
                int x, y;
                SDL_GetMouseState(&x, &y);
                bool pauseHover = (x >= 188 && x <= 328 && y >= 607 && y <= 647); // Updated hover detection
                SDL_Rect pauseButtonRect = {188, 607, 140, 40}; // Updated position and size of Pause button
                SDL_RenderCopy(renderer, pauseButtonTextures[pauseHover ? 1 : 0], nullptr, &pauseButtonRect);

                // Handle pause button click
                if (event.type == SDL_MOUSEBUTTONDOWN && pauseHover) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    gameState = PAUSE; // Transition to PAUSE state
                }

                // Handle game state transitions
                int alivePlayers = players.size(); // Assume all players are active
                int aliveBots = bots.size();       // Assume all bots are active

                if (alivePlayers + aliveBots == 1) {
                    if (alivePlayers > 0) {
                        gameState = YOU_WIN; // Player wins
                    } else {
                        gameState = YOU_LOSE; // Bot wins
                    }
                }

                SDL_RenderPresent(renderer);
            } else if (gameState == PAUSE) {
                // Load the pause screen texture
                SDL_Texture* pauseTexture = loadTexture(renderer, "assets/image/screens/pause.png");

                // Clear screen before rendering
                SDL_RenderClear(renderer);

                // Render the pause screen
                SDL_RenderCopy(renderer, pauseTexture, nullptr, nullptr);

                // Get mouse position
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Hover detection for buttons
                bool mainMenuHover = (x >= 403 && x <= 601 && y >= 624 && y <= 691); // Updated main menu button position
                bool resumeHover = (x >= 688 && x <= 886 && y >= 624 && y <= 691);   // Updated resume button position

                // Render main menu button
                SDL_Rect mainMenuButtonRect = { 403, 624, 198, 67 }; // Updated position and size for main menu button
                SDL_RenderCopy(renderer, mainMenuButtonTextures[mainMenuHover ? 1 : 0], nullptr, &mainMenuButtonRect);

                // Render resume button
                SDL_Rect resumeButtonRect = { 688, 624, 198, 67 }; // Updated position and size for resume button
                SDL_RenderCopy(renderer, resumeButtonTextures[resumeHover ? 1 : 0], nullptr, &resumeButtonRect);

                // Handle button clicks
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (mainMenuHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound

                        // Play menu music when transitioning to MENU
                        Mix_HaltMusic();
                        Mix_PlayMusic(menuMusic, -1);

                        gameState = MENU; // Transition to MENU
                    } else if (resumeHover) {
                        Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                        gameState = PLAYING; // Resume the game
                    }
                }

                // Present the rendered frame
                SDL_RenderPresent(renderer);

                // Free the pause screen texture after rendering
                SDL_DestroyTexture(pauseTexture);
            } else if (gameState == YOU_WIN || gameState == YOU_LOSE) {
                // Switch back to menu music if not already playing
                if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                    Mix_HaltMusic();
                    Mix_PlayMusic(menuMusic, -1);
                }

                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, clickSound, 0); // Play click sound
                    gameState = MENU; // Go back to main menu on click
                }

                if (gameState == YOU_WIN) {
                    // Switch to win music if not already playing
                    if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                        Mix_HaltMusic();
                        Mix_PlayMusic(winMusic, -1);
                    }
                    drawWinScreen(renderer, winTexture);
                } else {
                    // Switch to lose music if not already playing
                    if (Mix_PlayingMusic() == 0 || Mix_GetMusicType(NULL) != MUS_MP3) {
                        Mix_HaltMusic();
                        Mix_PlayMusic(loseMusic, -1);
                    }
                    drawLoseScreen(renderer, loseTexture);
                }
            }
        }

        // Ensure this block is properly closed
    }

    // Cleanup
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(playerTextures[i]);
    }
    SDL_DestroyTexture(menuTexture);
    for (int i = 0; i < 2; ++i) {
        SDL_DestroyTexture(startMenuButtonTextures[i]);
        SDL_DestroyTexture(howToPlayButtonTextures[i]);
        SDL_DestroyTexture(howToPlayBackButtonTextures[i]);
        SDL_DestroyTexture(startSettingsButtonTextures[i]);
        SDL_DestroyTexture(mainMenuButtonTextures[i]);
        SDL_DestroyTexture(resumeButtonTextures[i]);
        SDL_DestroyTexture(gameSettingsBackButtonTextures[i]);
        SDL_DestroyTexture(pauseButtonTextures[i]);
    }
    SDL_DestroyTexture(howToPlayTexture);
    SDL_DestroyTexture(gameSettingsTexture);
    for (int i = 0; i < 3; ++i) {
        SDL_DestroyTexture(introTextures[i]);
        SDL_DestroyTexture(basketballArenaTextures[i]);
        SDL_DestroyTexture(tombArenaTextures[i]);
    }
    for (int i = 0; i < 6; ++i) {
        SDL_DestroyTexture(basketballTextures[i]);
        SDL_DestroyTexture(tombTextures[i]);
    }
    SDL_DestroyTexture(leftPanelTexture);
    SDL_DestroyTexture(topFrameTexture);
    SDL_DestroyTexture(bottomFrameTexture);
    SDL_DestroyTexture(rightFrameTexture);
    Mix_FreeChunk(explosionSound);
    Mix_FreeChunk(clickSound);
    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(gameMusic);
    Mix_FreeMusic(winMusic);
    Mix_FreeMusic(loseMusic);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0; // Ensure the function ends with a return statement
} // Ensure this closing brace matches the opening brace of main
